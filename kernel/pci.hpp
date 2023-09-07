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

  /** @brief 指定されたPCIデバイスの32ビットレジスタを読み取る. */
  uint32_t ReadConfReg(const Device& dev,
                       uint8_t reg_addr);

  /** @brief 指定されたPCIデバイスの32ビットレジスタに書き込む. */
  void WriteConfReg(const Device& dev,
                    uint8_t reg_addr,
                    uint32_t value);

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

  WithError<uint64_t> ReadBar(Device& device,
                              unsigned int bar_index);

  /** @brief PCIケイパビリティレジスタの共通ヘッダ. */
  union CapabilityHeader {
    uint32_t data;
    struct {
      uint32_t cap_id : 8;
      uint32_t next_ptr : 8;
      uint32_t cap : 16;
    } __attribute__((packed)) bits;
  } __attribute__((packed));

  const uint8_t kCapabilityMSI = 0x05;
  const uint8_t kCapabilityMSIX = 0x11;

  /**
   * @brief 指定されたPCIの指定されたケイパビリティレジスタを読み込む.
   * 
   * @param dev ケイパビリティを読み込むPCIデバイス
   * @param addr ケイパビリティレジスタのコンフィグレーション空間アドレス
   */
  CapabilityHeader ReadCapabilityHeader(const Device& dev,
                                        uint8_t addr);

  /**
   * @brief MSIケイパビリティ構造.
   * 
   * MSIケイパビリティ構造は64ビットサポートの有無などで亜種がたくさんある.
   * この構造体は各亜種に対応するために最大の亜種に合わせてメンバを定義してある.
   */
  struct MSICapability {
    union {
      uint32_t data;
      struct {
        uint32_t cap_id : 8;
        uint32_t next_ptr : 8;
        uint32_t msi_enable : 1;
        uint32_t multi_msg_capable : 3;
        uint32_t multi_msg_enable : 3;
        uint32_t addr_64_capable : 1;
        uint32_t per_vector_mask_capable : 1;
        uint32_t : 7;
      } __attribute__((packed)) bits;
    } __attribute__((packed)) header;

    uint32_t msg_addr;
    uint32_t msg_upper_addr;
    uint32_t msg_data;
    uint32_t mask_bits;
    uint32_t pending_bits;
  } __attribute__((packed));

  /**
   * @brief MSIまたはMSI-X割り込みを設定する.
   * 
   * @param dev 設定対象のPCIデバイス
   * @param msg_addr 割り込み発生時にメッセージを書き込む先のアドレス
   * @param msg_data 割り込み発生時に書き込むメッセージの値
   * @param num_vector_exponent 割り当てるベクタ数（2^nのnを指定）
   */
  Error ConfigureMSI(const Device& dev,
                     uint32_t msg_addr,
                     uint32_t msg_data,
                     unsigned int num_vector_exponent);

  enum class MSITriggerMode {
    kEdge = 0,
    kLevel = 1
  };

  enum class MSIDeliveryMode {
    kFixed          = 0b000,
    kLowestPriority = 0b001,
    kSMI            = 0b010,
    kNMI            = 0b100,
    kINIT           = 0b101,
    kExtINT         = 0b111,
  };

  Error ConfigureMSIFixedDestination(const Device& dev,
                                     uint8_t apic_id,
                                     MSITriggerMode trigger_mode,
                                     MSIDeliveryMode delivery_mode,
                                     uint8_t vector,
                                     unsigned int num_vector_exponent);
}

void InitializePCI();
