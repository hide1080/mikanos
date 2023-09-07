/**
 * @file
 * セグメント関連.
 */

#include "asmfunc.h"
#include "segment.hpp"

namespace {
  std::array<SegmentDescriptor, 3> gdt;
}

void SetCodeSegment(SegmentDescriptor& desc,
                    DescriptorType type,
                    unsigned int descriptor_privilege_level,
                    uint32_t base,
                    uint32_t limit) {

  desc.data = 0;
  desc.bits.base_low = base & 0xffffU;
  desc.bits.base_middle = (base >> 16) & 0xffU;
  desc.bits.base_high = (base >> 24) & 0xffU;
  desc.bits.limit_low = limit & 0xffffU;
  desc.bits.limit_high = (limit >> 16) & 0xfU;
  desc.bits.type = type;
  desc.bits.system_segment = 1; // 1: code or data segment
  desc.bits.descriptor_privilege_level = descriptor_privilege_level;
  desc.bits.present = 1;
  desc.bits.available = 0;
  desc.bits.long_mode = 1;
  desc.bits.default_operation_size = 0; // should be 0 when long_mode == 1
  desc.bits.granularity = 1; // indicates segment length by page size
}

void SetDataSegment(SegmentDescriptor& desc,
                    DescriptorType type,
                    unsigned int descriptor_privilege_level,
                    uint32_t base,
                    uint32_t limit) {

  SetCodeSegment(
    desc,
    type,
    descriptor_privilege_level,
    base,
    limit
  );

  desc.bits.long_mode = 0;
  desc.bits.default_operation_size = 1; // 32-bit stack segment
}

void SetupSegments() {

  gdt[0].data = 0;

  SetCodeSegment(
    gdt[1],
    DescriptorType::kExecuteRead,
    0,
    0,
    0xfffff
  );

  SetDataSegment(
    gdt[2],
    DescriptorType::kReadWrite,
    0,
    0,
    0xfffff
  );

  LoadGDT(
    sizeof(gdt) - 1,
    reinterpret_cast<uintptr_t>(&gdt[0])
  );
}

void InitializeSegmentation() {
  SetupSegments();
  SetDSAll(kKernelDS);
  SetCSSS(kKernelCS, kKernelSS);
}
