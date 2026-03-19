#include "Shared/OctoDenReflectionHelpers.h"

#include "UObject/UnrealType.h"

FObjectProperty* OctoDenReflectionHelpers::FindRequiredObjectProperty(UObject& TargetObject, const TCHAR* PropertyName, UClass* ExpectedBaseClass, FText& OutFailure)
{
	FObjectProperty* ObjectProperty = FindFProperty<FObjectProperty>(TargetObject.GetClass(), PropertyName);
	if (ObjectProperty == nullptr)
	{
		OutFailure = FText::Format(
			NSLOCTEXT("OctoDenReflection", "MissingObjectProperty", "Required property '{0}' was not found on '{1}'."),
			FText::FromString(PropertyName),
			FText::FromString(TargetObject.GetClass()->GetPathName()));
		return nullptr;
	}

	if (ExpectedBaseClass != nullptr && !ObjectProperty->PropertyClass->IsChildOf(ExpectedBaseClass))
	{
		OutFailure = FText::Format(
			NSLOCTEXT("OctoDenReflection", "ObjectPropertyWrongType", "Property '{0}' on '{1}' does not accept '{2}'."),
			FText::FromString(PropertyName),
			FText::FromString(TargetObject.GetClass()->GetPathName()),
			FText::FromString(ExpectedBaseClass->GetName()));
		return nullptr;
	}

	return ObjectProperty;
}

bool OctoDenReflectionHelpers::SetObjectProperty(UObject& TargetObject, const TCHAR* PropertyName, UObject* Value, UClass* ExpectedBaseClass, FText& OutFailure)
{
	FObjectProperty* ObjectProperty = FindRequiredObjectProperty(TargetObject, PropertyName, ExpectedBaseClass, OutFailure);
	if (ObjectProperty == nullptr)
	{
		return false;
	}

	if (Value != nullptr && !Value->IsA(ObjectProperty->PropertyClass))
	{
		OutFailure = FText::Format(
			NSLOCTEXT("OctoDenReflection", "ObjectValueWrongType", "Value '{0}' is not compatible with '{1}'."),
			FText::FromString(Value->GetPathName()),
			FText::FromString(PropertyName));
		return false;
	}

	TargetObject.Modify();
	TargetObject.PreEditChange(ObjectProperty);
	ObjectProperty->SetObjectPropertyValue_InContainer(&TargetObject, Value);
	FPropertyChangedEvent ChangedEvent(ObjectProperty);
	TargetObject.PostEditChangeProperty(ChangedEvent);
	TargetObject.MarkPackageDirty();
	return true;
}
