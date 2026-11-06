"""测试性能报告的完整性与计算校验，不设置速度条件。"""
from pathlib import Path
import copy
import importlib.util
import math
import unittest

ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('current_report',ROOT/'benchmarks/current/report.py')
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)


def fixture():
    """用已知的 1001～1025 纳秒样本构造完整格式，而非实际性能数据。"""
    data=dict(cases=list(module.CASES),languages=list(module.LANGUAGES),runs=5,reps_per_process=5,completed='fixture',results={},summary={},builds={},oracle_values={})
    for case in module.CASES:
        data['oracle_values'][case]=[1,2,3,4,5]
        for language in module.LANGUAGES:
            key=case+'/'+language;sink='8.09375' if case=='float_chain' else '16'
            rows=[]
            for round_index in range(5):
                values=[1001+round_index*5+i for i in range(5)]
                rows.append(dict(round=round_index,code=0,stderr='',samples_ns=values,sink=sink,
                                 stdout=''.join('rep '+str(n)+'\n' for n in values)+'sink '+sink+'\n'))
            data['results'][key]=rows
            data['builds'][key]=dict(values=[1,2,3,4,5])
            data['summary'][key]=dict(samples=25,median_ms=1013/1e6,min_ms=1001/1e6,max_ms=1025/1e6)
    return data


class ReportChecks(unittest.TestCase):
    """拒绝缺失、无效观测及不一致的摘要。"""
    def test_complete(self):self.assertEqual(module.validate(fixture()),3600)

    def test_descriptive_statistics(self):
        stats=module.descriptive(fixture())['int_chain/c']
        self.assertAlmostEqual(stats['mean_ms'],1013/1e6)
        self.assertAlmostEqual(stats['sample_variance_ms2'],(650/12)/1e12,places=20)
        self.assertAlmostEqual(stats['sample_sd_ms'],math.sqrt(650/12)/1e6)
        self.assertAlmostEqual(stats['cv_percent'],math.sqrt(650/12)/1013*100)
        self.assertAlmostEqual(stats['adjusted_skewness'],0,places=10)
        self.assertEqual(stats['p25_ms'],1007/1e6)
        self.assertEqual(stats['p50_ms'],1013/1e6)
        self.assertEqual(stats['p75_ms'],1019/1e6)
        self.assertEqual(stats['p100_ms'],1025/1e6)

    def test_adjusted_skewness(self):
        data=fixture();key='int_chain/c'
        for row in data['results'][key]:
            row['samples_ns']=[1000,1000,1000,1000,3000]
            row['stdout']='rep 1000\n'*4+'rep 3000\nsink 16\n'
        data['summary'][key].update(median_ms=.001,min_ms=.001,max_ms=.003)
        self.assertAlmostEqual(module.descriptive(data)[key]['adjusted_skewness'],1.5*math.sqrt(25*24)/23)

    def test_constant_samples(self):
        data=fixture();key='int_chain/c'
        for row in data['results'][key]:
            row['samples_ns']=[1000]*5;row['stdout']='rep 1000\n'*5+'sink 16\n'
        data['summary'][key].update(median_ms=.001,min_ms=.001,max_ms=.001)
        stats=module.descriptive(data)[key]
        self.assertEqual(stats['sample_variance_ms2'],0)
        self.assertEqual(stats['cv_percent'],0)
        self.assertIsNone(stats['adjusted_skewness'])

    def test_bad_record(self):
        for change in ('missing_language','missing_process','missing_seed','zero','wrong_raw','wrong_sink','wrong_summary'):
            with self.subTest(change=change):
                data=fixture();key='int_chain/c'
                if change=='missing_language':data['languages'].pop()
                elif change=='missing_process':data['results'][key].pop()
                elif change=='missing_seed':data['builds'][key]['values'].pop()
                elif change=='zero':data['results'][key][0]['samples_ns'][0]=0
                elif change=='wrong_raw':data['results'][key][0]['stdout']='rep 999\nsink 16\n'
                elif change=='wrong_sink':data['results'][key][0]['sink']='17'
                else:data['summary'][key]['median_ms']=1.0
                with self.assertRaises(ValueError):module.validate(data)


if __name__=='__main__':unittest.main()
