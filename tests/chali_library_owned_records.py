"""验证原生库的 String 拥有字段、独立复制、借用及共享生命周期。"""
from pathlib import Path
import argparse
import copy
import os
import re
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode, encode


def main():
    """移走实现源码，通过两个原生单元核对拥有记录的复制与析构。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--external-clang', type=Path)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    tools = ['--llvm', args.external_clang.resolve()] if args.external_clang else []
    with tempfile.TemporaryDirectory(prefix='library-owned-records-') as directory:
        work = Path(directory)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        provider = work / 'provider.chl'
        package = modules / 'owned.chlib'
        provider.write_text('module demo::owned;\n'
                            'struct Message {String text;int64 marker;};\n'
                            'struct Bundle {Message first;Message second;String note;};\n'
                            'enum Event {Empty,Text(String text),Item(Message value)};\n'
                            'Message make(String text,int64 marker){Message value;value.text=text;value.marker=marker;return value;}\n'
                            'String read(Message const& value){return value.text;}\n'
                            'void append(Message mut& value){value.text=value.text+"!";value.marker=value.marker+1;}\n'
                            'Message echo(Message value){value.text=value.text+"!";return value;}\n'
                            'Message moved(Message value){Message result=move(value);result.marker=result.marker+2;return result;}\n'
                            'Message recurse(Message value,int64 depth){if(depth<=0){return value;}value.text=value.text+".";return recurse(value,depth-1);}\n'
                            '#borrow(#ret,[value])\n'
                            'Message const& view(Message const& value){return value;}\n'
                            '#borrow(#ret,[value])\n'
                            'Message mut& editable(Message mut& value){return value;}\n'
                            '#borrow(#ret,[value])\n'
                            'String const& textView(Message const& value){return &value.text;}\n'
                            'Bundle bundle(Message value){Bundle result;result.first=value;result.second=value;result.note="尾";return result;}\n'
                            'Bundle changed(Bundle value){value.first.text=value.first.text+"?";return value;}\n'
                            'Event empty(){return Event::Empty;}\n'
                            'Event textEvent(String text){return Event::Text(text);}\n'
                            'Event itemEvent(Message value){return Event::Item(value);}\n'
                            'Event eventEcho(Event value){return value;}\n'
                            'Event eventMoved(Event value){Event result=move(value);return result;}\n'
                            'String eventText(Event const& value){match(value){Event::Text(text)=>{return text;}Event::Item(item)=>{return item.text;}_=>{return "";}}}\n', encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::owned',
                '--package-version', '1.0.0', '--checked', '--with-bitcode', *tools, '--out', package], work)
        manifest, entries = decode(package.read_bytes())
        invoke([compiler, 'lib', 'inspect', package], work)
        assert manifest['targets'][0]['nativeAbi'] == 'chali-owned-v1'
        assert {'owned-record-value-v1', 'owned-string-value-v1', 'shared-type-id-v1'} <= set(manifest['requiredFeatures'])
        interface = next(entry['data'].decode() for entry in entries if entry['kind'] == 1)
        assert 'std::lang::String text;' in interface and '#abi("ChaLiOwned"' in interface
        provider_source = provider.read_text(encoding='utf-8')
        original_package = package.read_bytes()
        for target in ['linux-x64', 'linux-arm64']:
            invoke([compiler, 'lib', 'build', provider, '--module', 'demo::owned',
                    '--package-version', '1.0.0', '--checked', '--with-bitcode', '--target', target,
                    *tools, '--out', work / (target + '.chlib')], work)
        peer = modules / 'peer.chlib'
        provider.write_text('module demo::peer;\nstruct Message {String text;uint8 marker;};\n'
                            'Message make(){Message value;value.text="peer";value.marker=(uint8)9;return value;}\n', encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::peer',
                '--package-version', '1.0.0', '--checked', '--with-bitcode', *tools, '--out', peer], work)
        bridge = modules / 'bridge.chlib'
        provider.write_text('module demo::bridge;\nimport demo::owned;\n'
                            'demo::owned::Message forward(demo::owned::Message value){return demo::owned::echo(value);}\n'
                            'void edit(demo::owned::Message mut& value){demo::owned::append(value);}\n', encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::bridge',
                '--package-version', '1.0.0', '--checked', '--with-bitcode', *tools, '--out', bridge], work)
        provider.unlink()
        consumer = work / 'main.chl'
        (modules / 'other.chl').write_text('module demo::other;\nstruct Message {String text;int64 extra;};\n', encoding='utf-8')
        consumer.write_text('import demo::other;\nimport demo::bridge;\nimport demo::peer;\nimport std::memory;\n'
                            'void main(){demo::owned::Message original=demo::owned::make("hello",40);'
                            'demo::other::Message unrelated;unrelated.text="other";unrelated.extra=11;'
                            'demo::peer::Message peer=demo::peer::make();assert(peer.text=="peer");assert(peer.marker==(uint8)9);'
                            'demo::owned::Message copied=demo::bridge::forward(original);'
                            'assert(original.text=="hello");assert(original.marker==40);'
                            'assert(copied.text=="hello!");assert(copied.marker==40);'
                            'demo::bridge::edit(&original);assert(original.text=="hello!");assert(original.marker==41);'
                            '{demo::owned::Message const& view=demo::owned::view(&original);assert(view.text=="hello!");}'
                            'assert(demo::owned::read(&original)=="hello!");'
                            '{String const& text=demo::owned::textView(&original);assert(text=="hello!");}'
                            '{demo::owned::Message mut& edit=demo::owned::editable(&original);edit.text="edited";}'
                            'assert(original.text=="edited");assert(copied.text=="hello!");'
                            'demo::owned::Bundle bundle=demo::owned::bundle(copied);'
                            'demo::owned::Bundle changed=demo::owned::changed(bundle);'
                            'assert(bundle.first.text=="hello!");assert(changed.first.text=="hello!?");'
                            'assert(changed.second.text=="hello!");assert(bundle.note=="尾");'
                            'demo::owned::Event event=demo::owned::textEvent("payload");'
                            'demo::owned::Event eventCopy=demo::owned::eventEcho(event);'
                            'event=demo::owned::itemEvent(original);assert(demo::owned::eventText(&event)=="edited");'
                            'assert(demo::owned::eventText(&eventCopy)=="payload");'
                            'event=demo::owned::empty();assert(demo::owned::eventText(&event)=="");'
                            'demo::owned::Message moved=demo::owned::moved(original);assert(moved.text=="edited");assert(moved.marker==43);'
                            'assert(original.text=="edited");'
                            'demo::owned::Message recursive=demo::owned::recurse(original,3);assert(recursive.text=="edited...");'
                            'demo::owned::Event movedEvent=demo::owned::eventMoved(eventCopy);assert(demo::owned::eventText(&movedEvent)=="payload");'
                            'let before=std::memory::__memoryLiveBlocks();int64 i=0;while(i<30){'
                            'demo::owned::Message item=demo::owned::make("长字符串012345678901234567890123456789",i);'
                            'demo::owned::Message other=demo::bridge::forward(item);'
                            'assert(other.text=="长字符串012345678901234567890123456789!");'
                            'assert(item.text=="长字符串012345678901234567890123456789");'
                            'demo::owned::Bundle bundle=demo::owned::bundle(item);demo::owned::Bundle cloned=demo::owned::changed(bundle);'
                            'assert(bundle.first.text==item.text);assert(cloned.first.text==item.text+"?");'
                            'demo::owned::Event event=demo::owned::itemEvent(item);demo::owned::Event copiedEvent=demo::owned::eventMoved(event);'
                            'event=demo::owned::empty();assert(demo::owned::eventText(&copiedEvent)==item.text);i=i+1;}'
                            'assert(unrelated.text=="other");assert(peer.text=="peer");'
                            'assert(std::memory::__memoryLiveBlocks()==before);println("owning record library OK");}', encoding='utf-8')
        for implementation, mode in [('native', 'off'), ('bitcode', 'off'), ('bitcode', 'full'), ('bitcode', 'thin')]:
            for allocator in ['malloc', 'pool']:
                result = invoke([compiler, 'run', consumer, '--allocator', allocator,
                                 '--library-implementation', implementation, '--lto-mode', mode, *tools,
                                 '--out', work / (implementation + '-' + mode + '-' + allocator)], work)
                assert result.stdout == 'owning record library OK\n'
        output = work / 'unchecked'
        invoke([compiler, 'aot', consumer, *tools, '--out', output], work)
        executable = output.with_suffix('.exe') if os.name == 'nt' else output
        assert invoke([executable], work).stdout == 'owning record library OK\n'
        obj = work / 'detached'
        invoke([compiler, 'aot', consumer, '--emit-obj', *tools, '--out', obj], work)
        linked = work / ('linked.exe' if os.name == 'nt' else 'linked')
        invoke([compiler, 'link', obj.with_suffix('.o'), '--package', bridge, '--package', peer, *tools, '--out', linked], work)
        assert invoke([linked], work).stdout == 'owning record library OK\n'
        probe = work / 'probe.chl'
        probe.write_text('import demo::owned;\nvoid main(){demo::owned::Message value=demo::owned::make("x",1);assert(value.text=="x");}', encoding='utf-8')
        invoke([compiler, 'check', probe], work)
        malformed = copy.deepcopy(manifest)
        malformed['requiredFeatures'].remove('owned-record-value-v1')
        package.write_bytes(encode(malformed, entries))
        invoke([compiler, 'lib', 'inspect', package], work)
        rejected = invoke([compiler, 'check', probe], work, False)
        assert 'owned-record-value-v1' in rejected.stderr, rejected.stderr
        for feature in ['owned-string-value-v1', 'shared-type-id-v1']:
            malformed = copy.deepcopy(manifest)
            malformed['requiredFeatures'].remove(feature)
            package.write_bytes(encode(malformed, entries))
            rejected = invoke([compiler, 'lib', 'inspect', package], work, False)
            assert '拥有字段聚合缺少 String 或共享类型身份能力' in rejected.stderr, rejected.stderr
        package.write_bytes(original_package)
        for source in ['import demo::owned;\nvoid main(){demo::owned::Message value=demo::owned::make("x",1);demo::owned::Message const& view=&value;demo::owned::append(view);}',
                       'import demo::owned;\ndemo::owned::Message const& escape(){demo::owned::Message value=demo::owned::make("x",1);return demo::owned::view(&value);}void main(){}',
                       'import demo::owned;\nimport demo::peer;\nvoid main(){demo::owned::Message value=demo::peer::make();}',
                       'import demo::owned;\nvoid main(){demo::owned::Message first=demo::owned::make("x",1);demo::owned::Message second=move(first);println(first.text);}']:
            probe.write_text(source, encoding='utf-8')
            rejected = invoke([compiler, 'check', probe], work, False)
            assert 'Undeclared' not in rejected.stderr and '未声明' not in rejected.stderr, rejected.stderr
        probe.write_text('import demo::owned;\nvoid main(){demo::owned::Message value=demo::owned::make("x",1);assert(value.text=="x");}', encoding='utf-8')
        stale = work / 'stale'
        invoke([compiler, 'aot', probe, '--emit-obj', *tools, '--out', stale], work)
        provider.write_text(provider_source.replace('struct Message {String text;', 'struct Message {int64 pad;String text;'), encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::owned',
                '--package-version', '2.0.0', '--checked', *tools, '--out', package], work)
        changed_interface = next(entry['data'].decode() for entry in decode(package.read_bytes())[1] if entry['kind'] == 1)
        assert re.findall(r'chali_lib_[0-9a-f]+', interface) != re.findall(r'chali_lib_[0-9a-f]+', changed_interface)
        linked.write_bytes(b'preserved executable')
        invoke([compiler, 'link', stale.with_suffix('.o'), '--package', package, *tools, '--out', linked], work, False)
        assert linked.read_bytes() == b'preserved executable'
        assert invoke([compiler, 'run', probe, *tools, '--out', work / 'new-layout'], work).returncode == 0
    print('owning record library: shared String fields, independent copies, borrows and cleanup OK')


if __name__ == '__main__':
    main()
