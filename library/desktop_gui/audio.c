#include "desktop.h"
#include "raylib.h"
#include <stdint.h>

typedef struct
{
    int64_t id;
    Sound sound;
} SoundSlot;

static SoundSlot sounds[64];
static int64_t next_id = 1;

/**
 * @brief 查找当前设备拥有的声音。
 * @param id 由 load 返回的有效标识。
 * @return 对应槽位，找不到时返回空指针。
 */
static SoundSlot *find_sound(int64_t id)
{
    if (id <= 0 || !IsAudioDeviceReady()) return 0;
    for (unsigned index = 0; index < sizeof sounds / sizeof *sounds; ++index)
        if (sounds[index].id == id) return &sounds[index];
    return 0;
}

/**
 * @brief 初始化当前进程的音频设备。
 * @return 音频设备就绪时为真。
 */
CHALI_GUI_API bool chali_audio_open(void)
{
    if (!IsAudioDeviceReady()) InitAudioDevice();
    return IsAudioDeviceReady();
}

/**
 * @brief 从磁盘解码短声音并登记。
 * @param path 由 raylib 支持的音频文件路径。
 * @return 声音标识，设备不可用、文件错误或槽位耗尽时返回零。
 */
CHALI_GUI_API int64_t chali_audio_load(const char *path)
{
    if (!IsAudioDeviceReady() || !FileExists(path) || next_id == INT64_MAX) return 0;
    for (unsigned index = 0; index < sizeof sounds / sizeof *sounds; ++index)
    {
        SoundSlot *slot = &sounds[index];
        if (slot->id != 0) continue;
        Sound sound = LoadSound(path);
        if (!IsSoundValid(sound)) return 0;
        slot->sound = sound;
        return slot->id = next_id++;
    }
    return 0;
}

/**
 * @brief 播放已登记的声音。
 * @param id 声音标识。
 * @return 调用有效时为真。
 */
CHALI_GUI_API bool chali_audio_play(int64_t id)
{
    SoundSlot *slot = find_sound(id);
    if (!slot) return false;
    PlaySound(slot->sound);
    return true;
}

/**
 * @brief 停止播放。
 * @param id 声音标识。
 * @return 调用有效时为真。
 */
CHALI_GUI_API bool chali_audio_stop(int64_t id)
{
    SoundSlot *slot = find_sound(id);
    if (!slot) return false;
    StopSound(slot->sound);
    return true;
}

/**
 * @brief 查询声音是否仍在播放。
 * @param id 声音标识。
 * @return 正在播放时为真。
 */
CHALI_GUI_API bool chali_audio_is_playing(int64_t id)
{
    SoundSlot *slot = find_sound(id);
    return slot && IsSoundPlaying(slot->sound);
}

/**
 * @brief 释放声音并使其标识失效。
 * @param id 声音标识。
 * @return 释放有效声音时为真。
 */
CHALI_GUI_API bool chali_audio_unload(int64_t id)
{
    SoundSlot *slot = find_sound(id);
    if (!slot) return false;
    StopSound(slot->sound);
    UnloadSound(slot->sound);
    slot->id = 0;
    return true;
}

/**
 * @brief 释放已登记的声音，然后关闭设备。
 */
CHALI_GUI_API void chali_audio_close(void)
{
    if (!IsAudioDeviceReady()) return;
    for (unsigned index = 0; index < sizeof sounds / sizeof *sounds; ++index)
        if (sounds[index].id) chali_audio_unload(sounds[index].id);
    CloseAudioDevice();
}
