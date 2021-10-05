#pragma once

#include <bal/types.h>

BrResult br_ev_req(BrId to, BrMsg *req, BrMsg *resp);

BrResult br_ev_resp(BrMsg const *req, BrMsg *resp);

int br_ev_run(void);

void br_ev_exit(int value);
