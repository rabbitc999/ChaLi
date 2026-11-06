"""导出本次实际样本、采样时源码和构件；不改变结果或重新计时。"""
from pathlib import Path
import argparse
import csv
import hashlib
import json
import shutil
import sys
import zipfile

ROOT=Path(__file__).resolve().parents[1]


def digest(path):
    """返回真实字节摘要。"""
    with path.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()


def main():
    """先保存完整快照，再生成可独立下载的归档及 Markdown。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report',type=Path,required=True)
    parser.add_argument('--compiler',type=Path,required=True)
    parser.add_argument('--out',type=Path,required=True)
    parser.add_argument('--refresh-analysis',action='store_true')
    args=parser.parse_args()
    report=json.loads(args.report.read_text(encoding='utf-8'))
    sys.path.insert(0,str(ROOT/'benchmarks/current'))
    from report import validate, descriptive
    count=validate(report)
    audit=args.report.with_name('identity-audit.json')
    if not json.loads(audit.read_text(encoding='utf-8')).get('after_measurement_identical'):raise ValueError('Identity audit incomplete')
    if digest(args.compiler)!=report['compiler_sha256']:raise ValueError('Measured compiler bytes unavailable')
    stage=args.report.parent/'snapshot'
    existing=stage.exists()
    if existing and not args.refresh_analysis:raise FileExistsError(stage)
    if existing:
        if digest(stage/'results.json')!=digest(args.report):raise ValueError('Original observations changed')
        if digest(stage/'compiler/chali-1.0.0-measured-windows-x64.exe')!=report['compiler_sha256']:raise ValueError('Archived compiler changed')
        records=json.loads((stage/'artifact-manifest.json').read_text(encoding='utf-8'))['files']
        for relative,entry in records.items():
            if digest(stage/relative)!=entry['sha256']:raise ValueError(('Archived file changed',relative))
    else:
        stage.mkdir()
        records={}

    def copy(source,relative):
        """复制明确文件并记录其原路径和摘要。"""
        source=Path(source)
        target=stage/relative;target.parent.mkdir(parents=True,exist_ok=True)
        shutil.copy2(source,target)
        records[target.relative_to(stage).as_posix()]=dict(original_path=str(source),sha256=digest(target))

    if not existing:
        copy(args.report,'results.json');copy(audit,'identity-audit.json')
        copy(args.compiler,'compiler/chali-1.0.0-measured-windows-x64.exe')
        for name,expected in report['source_sha256'].items():
            source=ROOT/name
            if digest(source)!=expected:raise ValueError(('Measured source changed',name))
            copy(source,'sources/'+name)
        scratch=Path(report['builds']['int_chain/chali']['folder']).parents[1]
        resolved=scratch.resolve()
        if resolved.parent!=(ROOT/'tmp').resolve() or not resolved.name.startswith('current-benchmark-'):raise ValueError('Unexpected artifact root')
        for source in sorted(scratch.rglob('*')):
            if source.is_file():copy(source,'artifacts/'+source.relative_to(scratch).as_posix())
        copy(report['tools']['lua']['path'],'runtime/luabench.exe')
        for name in ('LICENSE','NOTICE'):
            if (ROOT/name).is_file():copy(ROOT/name,'licenses/ChaLi/'+name)
        copy(ROOT/'library/lua/LICENSE.txt','licenses/Lua-LICENSE.txt')
        go_license=Path(report['tools']['go']['path']).parent.parent/'LICENSE'
        if go_license.is_file():copy(go_license,'licenses/Go-LICENSE.txt')
        rust_root=Path(report['tools']['rust']['path']).parent.parent
        for folder in (rust_root,rust_root/'share/doc/rust'):
            for name in ('LICENSE-APACHE','LICENSE-MIT','COPYRIGHT'):
                if (folder/name).is_file():copy(folder/name,'licenses/Rust-'+name)
        vsix=ROOT/'vscode/dist/chali-editor-1.0.0-llvm-static.vsix'
        with zipfile.ZipFile(vsix) as z:
            for name in z.namelist():
                if name.startswith('extension/licenses/') and not name.endswith('/'):
                    relative=Path('licenses/compiler')/Path(name).relative_to('extension/licenses')
                    target=stage/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(z.read(name))
                    records[relative.as_posix()]=dict(original_path=str(vsix)+'!'+name,sha256=digest(target))
    copy(ROOT/'benchmarks/current/report.py','analysis-source/report.py')
    copy(Path(__file__),'analysis-source/export_performance_data.py')
    formatting=args.report.parent.parent/'source-format-verification.json'
    if formatting.is_file():copy(formatting,'analysis-source/source-format-verification.json')
    with (stage/'samples.csv').open('w',encoding='utf-8',newline='') as f:
        writer=csv.writer(f);writer.writerow(['workload','language','process','seed','nanoseconds'])
        for key,rows in report['results'].items():
            case,language=key.split('/')
            for row in rows:
                for seed,value in enumerate(row['samples_ns']):writer.writerow([case,language,row['round']+1,seed,value])
    statistics=descriptive(report)
    fields=['samples','min_ms','median_ms','max_ms','mean_ms','p25_ms','p50_ms','p75_ms','p100_ms','sample_variance_ms2','sample_sd_ms','cv_percent','adjusted_skewness']
    with (stage/'summary.csv').open('w',encoding='utf-8',newline='') as f:
        writer=csv.writer(f);writer.writerow(['workload','language',*fields])
        for key,row in statistics.items():writer.writerow([*key.split('/'),*[row[name] for name in fields]])
    (stage/'statistics.json').write_text(json.dumps(dict(derived_from_sha256=digest(args.report),variance='sample, n-1',
        quantiles='R7: p25, p50, p75, p100',cv='100 * sample_sd / mean',skewness='adjusted Fisher-Pearson G1',
        analysis_source_sha256=digest(ROOT/'benchmarks/current/report.py'),statistics=statistics),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    (stage/'artifact-manifest.json').write_text(json.dumps(dict(files=records,external_tools=report['tools']),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    (stage/'README.txt').write_text('ChaLi 1.0.0 performance measurements\n\n144 combinations; 3600 timed observations.\nresults.json retains all original outputs and samples. samples.csv is a flat view.\nsummary.csv and statistics.json contain minimum, maximum, median, mean, P25/P50/P75/P100 (R7), sample variance (n-1), SD, CV (%) and adjusted Fisher-Pearson skewness.\nsources/ is the exact source snapshot at measurement time; artifacts/ contains measured kernels, timing wrappers and generated code.\nanalysis-source/ contains the current statistics and export scripts. Later comment/format changes are checked separately and do not change the measured kernels.\ncompiler/ contains the exact measured ChaLi executable, identified by SHA-256.\nJava and Python runtimes and the development toolchains remain external; their versions and identities are recorded.\nThis archive contains no reused earlier timings and no speed-based pass/fail classification.\n',encoding='utf-8')
    checks=[]
    for source in sorted(stage.rglob('*')):
        if source.is_file() and source.name!='SHA256SUMS.txt' and '__pycache__' not in source.parts:checks.append(digest(source)+'  '+source.relative_to(stage).as_posix()+'\n')
    (stage/'SHA256SUMS.txt').write_text(''.join(checks),encoding='utf-8')
    args.out.mkdir(parents=True,exist_ok=True)
    shutil.copy2(stage/'summary.csv',args.out/'ChaLi-1.0.0-performance-summary.csv')
    archive=args.out/'ChaLi-1.0.0-performance-data.zip'
    with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED,compresslevel=6) as z:
        for source in sorted(stage.rglob('*')):
            if source.is_file() and '__pycache__' not in source.parts:z.write(source,source.relative_to(stage).as_posix())
    for name,target in [('doc/12-performance.md','12-performance.md'),('doc/en/12-performance.md','12-performance.en.md')]:
        text=(ROOT/name).read_text(encoding='utf-8')
        text=text.replace('../../benchmarks/current/','https://github.com/rabbitc999/ChaLi/blob/master/benchmarks/current/').replace('../benchmarks/current/','https://github.com/rabbitc999/ChaLi/blob/master/benchmarks/current/')
        text=text.replace('(en/12-performance.md)','(12-performance.en.md)').replace('(../12-performance.md)','(12-performance.md)')
        (args.out/target).write_text(text,encoding='utf-8')
    print('Exported',count,'observations:',archive)


if __name__=='__main__':main()
