/**
 * @file memory_manager.hpp
 * 
 * メモリ管理関連.
 */

#pragma once

#include <array>
#include <limits>
#include "error.hpp"
#include "memory_map.hpp"

namespace {
  constexpr unsigned long long operator ""_KiB(unsigned long long kib) {
    return kib * 1024;
  }

  constexpr unsigned long long operator ""_MiB(unsigned long long mib) {
    return mib * 1024_KiB;
  }

  constexpr unsigned long long operator ""_GiB(unsigned long long gib) {
    return gib * 1024_MiB;
  }
}

/** @brief 物理メモリ1つの大きさ（バイト）. */
static const auto kBytesPerFrame { 4_KiB };

class FrameID {
  public:
    explicit FrameID(size_t id) : id_{id} {}

    size_t ID() const {
      return id_;
    }

    void* Frame() const {
      return reinterpret_cast<void*>(id_ * kBytesPerFrame);
    }

  private:
    size_t id_;
};

static const FrameID kNullFrame { std::numeric_limits<size_t>::max() };

struct MemoryStat {
  size_t allocated_frames;
  size_t total_frames;
};

/**
 * @brief ビットマップ配列を用いてフレーム単位でメモリ管理するクラス.
 * 
 * 1ビットを1フレームに対応させて、ビットマップにより空きフレームを管理する.
 * 配列alloc_mapの各ビットがフレームに対応し、0なら空き、1なら使用中.
 * alloc_map[n]のmビット目が対応する物理アドレスは次の式で求まる:
 *   kFrameBytes * (n * kBitsPerMapLine + m)
 */
class BitmapMemoryManager {
  public:
    /** @brief このメモリ管理クラスで扱える最大の物理メモリ量（バイト）. */
    static const auto kMaxPhysicalMemoryBytes { 128_GiB };

    /** @brief kMaxPhysicalMemoryBytesまでの物理メモリを扱うために必要なフレーム数. */
    static const auto kFrameCount { kMaxPhysicalMemoryBytes / kBytesPerFrame };

    /** @brief ビットマップ配列の要素型. */
    using MapLineType = unsigned long;

    /** @brief ビットマップ配列の1つの要素のビット数 == フレーム数. */
    static const size_t kBitsPerMapLine { 8 * sizeof(MapLineType) };

    /** @brief インスタンスを初期化する. */
    BitmapMemoryManager();

    /** @brief 要求されたフレーム数の領域を確保して先頭のフレームIDを返す. */
    WithError<FrameID> Allocate(size_t num_frames);

    Error Free(FrameID start_frame, size_t num_frames);

    void MarkAllocated(FrameID start_frame, size_t num_frames);

  /**
   * @brief このメモリマネージャで扱うメモリ範囲を設定する.
   * 
   * この呼び出し以降、Allocateによるメモリ割り当ては設定された範囲内でのみ行われる.
   * 
   * @param range_begin_ メモリ範囲の視点
   * @param range_end_   メモリ範囲の終点. 最終フレームの次のフレーム
   */
  void SetMemoryRange(FrameID range_begin, FrameID range_end);

  /**
   * @brief 空き／総フレームの数を返す.
   */
  MemoryStat Stat() const;

  private:
    std::array<MapLineType, kFrameCount / kBitsPerMapLine> alloc_map_;

    /** @brief このメモリマネージャで扱うメモリ範囲の視点. */
    FrameID range_begin_;
    /** @brief このメモリマネージャでメモリ範囲の終点. 最終フレームの次のフレーム. */
    FrameID range_end_;

    bool GetBit(FrameID frame) const;
    void SetBit(FrameID frame, bool allocated);
};

extern BitmapMemoryManager* memory_manager;

void InitializeMemoryManager(const MemoryMap& memory_map);
