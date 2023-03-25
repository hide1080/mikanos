#pragma once

#include <cstdint>
#include <array>

#include "error.hpp"

namespace pci {

  /** CONFIG_ADDRESSレジスタのIOポートアドレス. */
  const uint16_t kConfigAddress = 0x0cf8;

  /** CONFIG_DATAレジスタのIOポートアドレス. */
  const uint16_t kConfigData = 0x0cfc;

  /** PCIデバイスのクラスコード */
  struct ClassCode {
    uint8_t base, sub, interface;

    /** ベースクラスが等しい場合、真. */
    bool Match(uint8_t b) {
      return b == base;
    }

    /** ベースクラスとサブクラスが等しい場合、真. */
    bool Match(uint8_t b, uint8_t s) {
      return Match(b) && s == sub;
    }

    /** ベースクラス、サブクラス、インタフェースが等しい場合、真. */
    bool Match(uint8_t b, uint8_t s, uint8_t i) {
      return Match(b, s) && i == interface;
    }
  };

  struct Device {
    uint8_t bus, device, function, header_type;
    ClassCode class_code;
  };

  /** CONFIG_ADDRESSレジスタに指定された整数を書き込む. */
  void WriteAddress(uint32_t address);

  /** CONFIG_DATAレジスタに指定された整数を書き込む. */
  void WriteData(uint32_t value);

  /** CONFIG_DATAレジスタから整数を読み込む. */
  uint32_t ReadData();

  /** ベンダIDレジスタを読み取る（全ヘッダタイプ共通）. */
  uint16_t ReadVendorId(uint8_t bus,
                        uint8_t device,
                        uint8_t function);

  /** デバイスIDレジスタを読み取る（全ヘッダタイプ共通）. */
  uint16_t ReadDeviceId(uint8_t bus,
                        uint8_t device,
                        uint8_t function);

  /** ヘッダタイプIDレジスタを読み取る（全ヘッダタイプ共通）. */
  uint8_t ReadHeaderType(uint8_t bus,
                        uint8_t device,
                        uint8_t function);

  /**
   * クラスコードレジスタを読み取る（全ヘッダタイプ共通）.
   *
   * - 31:24 : ベースクラス
   * - 23:16 : サブクラス
   * - 15: 8 : インタフェース
   * -  7: 0 : リビジョン
   */
  ClassCode ReadClassCode(uint8_t bus,
                        uint8_t device,
                        uint8_t function);

  inline uint16_t ReadVendorId(const Device& dev) {
    return ReadVendorId(dev.bus, dev.device, dev.function);
  }

  /** 指定されたPCIデバイスの32ビットレジスタを読み取る. */
  uint32_t ReadConfReg(const Device& dev, uint8_t reg_addr);

  void WriteConfReg(const Device& dev, uint8_t reg_addr, uint32_t value);

  /**
   * バス番号レジスタを読み取る（ヘッダタイプ1用）.
   *
   * - 23:16 : サブオーディネイトバス番号
   * - 15: 8 : セカンダリバス番号
   * -  7: 0 : リビジョン
   */
  uint8_t ReadBusNumbers(uint8_t bus,
                        uint8_t device,
                        uint8_t function);

  bool IsSingleFunctionDevice(uint8_t header_type);

  /** 検出したPCIデバイスのリスト. */
  inline std::array<Device, 32> devices;

  /** 検出したPCIデバイスの数. */
  inline int num_device;

  /** バス0から再帰的にPCIデバイスを探索してdevicesに格納する. */
  Error ScanAllBus();

  constexpr uint8_t CalcBarAddress(unsigned int bar_index) {
    return 0x10 + 4 * bar_index;
  }

  WithError<uint64_t> ReadBar(Device& device, unsigned int bar_index);
}