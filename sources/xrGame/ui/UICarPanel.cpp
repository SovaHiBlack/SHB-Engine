#include "stdafx.h"

#include "UIMainIngameWnd.h"//
#include "UICarPanel.h"//
#include "UIXmlInit.h"//

const char* CAR_PANEL_XML = "car_panel.xml";
const char* POINTER_ARROW_TEX = "ui\\hud_map_arrow";

void CUICarPanel::Init(float x, float y, float width, float height)
{
	CUIXml uiXml;
	bool result = uiXml.Init("$game_config$", "ui", CAR_PANEL_XML);
	R_ASSERT3(result, "xml file not found", CAR_PANEL_XML);

	CUIXmlInit xml_init;
	AttachChild(&UIStaticCarHealth);
	xml_init.InitStatic(uiXml, "car_health_static", 0, &UIStaticCarHealth);

	UIStaticCarHealth.AttachChild(&UICarHealthBar);
	xml_init.InitProgressBar(uiXml, "car_health_progress_bar", 0, &UICarHealthBar);

	Show(false);
	Enable(false);

	inherited::Init(x, y, width, height);
}

void CUICarPanel::SetCarHealth(float value)
{
	float pos = value * 100;
	clamp(pos, 0.0f, 100.0f);
	UICarHealthBar.SetProgressPos(pos);
}

void CUICarPanel::SetSpeed(float speed)
{ }

void CUICarPanel::SetRPM(float rpm)
{ }
