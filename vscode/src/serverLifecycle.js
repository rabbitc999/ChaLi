'use strict';

/** 同一扩展最多进行一次服务切换；排队重启只执行最新请求。 */
class ServerLifecycle {
  /** @param {object} actions 创建服务和停止当前服务的回调。 */
  constructor(actions) {
    this.actions = actions;
    this.transition = Promise.resolve();
    this.version = 0;
    this.closed = false;
  }

  /** @param {object} context 最新的扩展上下文。 @returns {Promise<void>} 切换完成。 */
  restart(context) {
    const version = ++this.version;
    const pending = this.transition.then(async () => {
      if (!this.closed && version === this.version) await this.actions.start(context);
    });
    this.transition = pending.catch(() => {});
    return pending;
  }

  /** 停用后不再启动排队任务，并等待正在进行的切换和最终进程回收。 */
  async dispose() {
    this.closed = true;
    await this.transition;
    await this.actions.stop();
  }
}

module.exports = { ServerLifecycle };
