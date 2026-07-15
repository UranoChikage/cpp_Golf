#pragma once

/***************************************************************
 * 2026/07/02
 * \‘¢‘Ì–¼FContact
 * ÚG‚µ‚Ä‚é‚©‚ğ}‚é‚½‚ß‚É’è‹`
 * *************************************************************/
struct Contact
{
public:
	bool isHit;
	XMFLOAT3 point;
	XMFLOAT3 normal;
	float penetration; // ‚ß‚è‚İ—Ê
	IHitReceiver* hit;
	bool isTigger;
};