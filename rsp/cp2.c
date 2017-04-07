//
// rsp/cp2.c: RSP control coprocessor.
//
// CEN64: Cycle-Accurate Nintendo 64 Emulator.
// Copyright (C) 2015, Tyler J. Stachecki.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#include "common.h"
#include "rsp/cpu.h"
#include "rsp/rsp.h"

//
// CFC2
//
void RSP_CFC2(struct rsp *rsp,
  uint32_t iw, uint32_t rs, uint32_t rt) {
  struct rsp_exdf_latch *exdf_latch = &rsp->pipeline.exdf_latch;
  struct rsp_cp2 *cp2 = &rsp->cp2;
  unsigned rd, dest, src;

  dest = GET_RT(iw);
  rd = GET_RD(iw);

  // TODO: verify on hardware
  if ((src = rd & 0x3) == 0x3)
    src = 2;

  exdf_latch->result.result = rsp_get_flags(cp2->flags[src].e);
  exdf_latch->result.dest = dest;
}

//
// CTC2
//
void RSP_CTC2(struct rsp *rsp,
  uint32_t iw, uint32_t rs, uint32_t rt) {
  struct rsp_cp2 *cp2 = &rsp->cp2;
  unsigned rd, dest;

  rd = GET_RD(iw);

  // TODO: verify on hardware
  if ((dest = rd & 0x3) >= 0x2) {
    rt &= 0xFF;
    dest = 2;
  }

  rsp_set_flags(cp2->flags[dest].e, rt);
}

//
// MFC2
//
void RSP_MFC2(struct rsp *rsp,
  uint32_t iw, uint32_t rs, uint32_t rt) {
  struct rsp_exdf_latch *exdf_latch = &rsp->pipeline.exdf_latch;
  const uint16_t *e = rsp->cp2.regs[GET_RD(iw)].e;
  unsigned dest, element = GET_EL(iw);
  unsigned lo = element >> 1;
  uint32_t data;

  uint16_t high;
  uint8_t low;

  dest = GET_RT(iw);

  if (element & 0x1) {
    unsigned hi = (element + 1) >> 1;

    high = e[lo] << 8;
    low = e[hi] >> 8;

    data = (int16_t) (high | low);
  }

  else
    data = (int16_t) e[lo];

  exdf_latch->result.result = data;
  exdf_latch->result.dest = dest;
}

//
// MTC2
//
void RSP_MTC2(struct rsp *rsp,
  uint32_t iw, uint32_t rs, uint32_t rt) {
  uint16_t *e = rsp->cp2.regs[GET_RD(iw)].e;
  unsigned element = GET_EL(iw);
  unsigned lo = element >> 1;

  if (element & 0x1) {
    unsigned hi = (element + 1) >> 1;

    e[lo] = (e[lo] & 0xFF00) | (rt >> 8 & 0xFF);
    e[hi] = (e[hi] & 0x00FF) | ((rt & 0xFF) << 8);
  }

  else
    e[lo] = rt;
}

// Initializes the coprocessor.
void rsp_cp2_init(struct rsp *rsp) {

}

