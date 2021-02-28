#pragma once

class CPhraseDialog;

using DIALOG_SHARED_PTR = intrusive_ptr<CPhraseDialog>;

//DEFINE_VECTOR(shared_str, DIALOG_ID_VECTOR, DIALOG_ID_IT);
using DIALOG_ID_VECTOR = xr_vector<shared_str>;
using DIALOG_ID_IT = DIALOG_ID_VECTOR::iterator;

#include "PhraseDialog.h"
