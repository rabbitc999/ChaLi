"""兼容入口：从锁定上游下载依赖，不在源码目录保存 SDK 或归档。"""
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))
from fetch_lua import main

if __name__ == "__main__":
    main()
