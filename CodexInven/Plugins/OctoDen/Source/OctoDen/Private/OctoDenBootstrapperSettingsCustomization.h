#pragma once

#include "IDetailCustomization.h"

class FOctoDenBootstrapperSettingsCustomization final : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
