'use strict';
/** 插件测试的临时文件夹与子进程统一使用仓库 tmp。 */
const fs = require('node:fs');
const path = require('node:path');
const repository = fs.realpathSync(path.resolve(__dirname, '../..'));
const root = path.join(repository, 'tmp');
fs.mkdirSync(root, { recursive: true });
if (path.dirname(fs.realpathSync(root)) !== repository) {
    throw new Error('项目 tmp 不能重定向到仓库外部');
}
for (const name of ['TMPDIR', 'TMP', 'TEMP']) process.env[name] = root;
module.exports = { root };
