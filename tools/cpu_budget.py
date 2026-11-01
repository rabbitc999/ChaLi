"""只在 CPU 占用低于 40% 时允许 LSP 计时；功能检查始终执行。"""
import os
from pathlib import Path
import shutil
import subprocess
import time


def timing_budget():
    """读取计时区间外的系统占用，无法可靠读取时保守地延后计时。"""
    if os.environ.get('CHALI_SKIP_LSP_TIMING') == '1':
        return dict(allowed=False, cpuPercent=None, reason='explicitly deferred')
    try:
        if os.name == 'nt':
            shell = shutil.which('pwsh') or 'powershell'
            result = subprocess.run([shell, '-NoProfile', '-Command',
                                     '(Get-CimInstance Win32_Processor | Measure-Object LoadPercentage -Average).Average'],
                                    capture_output=True, text=True, encoding='utf-8', timeout=15)
            if result.returncode:
                raise RuntimeError(result.stderr.strip())
            usage = float(result.stdout.strip().replace(',', '.'))
        elif Path('/proc/stat').is_file():
            def sample():
                """读取 Linux CPU 累计时钟。"""
                values = list(map(int, Path('/proc/stat').read_text().splitlines()[0].split()[1:9]))
                return sum(values), values[3] + values[4]
            first, idle_first = sample()
            time.sleep(0.25)
            last, idle_last = sample()
            usage = 100 * (1 - (idle_last - idle_first) / max(1, last - first))
        else:
            raise RuntimeError('system CPU counter is unavailable')
        return dict(allowed=usage < 40, cpuPercent=round(usage, 2), reason='' if usage < 40 else 'CPU >= 40%')
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        return dict(allowed=False, cpuPercent=None, reason=str(error))
