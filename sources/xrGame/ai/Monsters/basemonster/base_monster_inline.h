#pragma once

IC void CBaseMonster::set_custom_panic_threshold(f32 value)
{
	m_panic_threshold = value;
}

IC void CBaseMonster::set_default_panic_threshold()
{
	m_panic_threshold = m_default_panic_threshold;
}
