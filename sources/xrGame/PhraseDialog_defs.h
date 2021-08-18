#pragma once

class CPhraseDialog;

using DIALOG_SHARED_PTR = intrusive_ptr<CPhraseDialog>;

//DEFINE_VECTOR(CSharedString, DIALOG_ID_VECTOR, DIALOG_ID_IT);
using DIALOG_ID_VECTOR = xr_vector<CSharedString>;
using DIALOG_ID_IT = DIALOG_ID_VECTOR::iterator;

#include "PhraseDialog.h"
