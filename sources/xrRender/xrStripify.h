#pragma once

void	xrStripify	(xr_vector<unsigned short> &indices, xr_vector<unsigned short> &perturb, int iCacheSize, int iMinStripLength);
int		xrSimulate	(xr_vector<unsigned short> &indices, int iCacheSize);
