/*
 * PROPRIETARY INFORMATION.  This software is proprietary to
 * Side Effects Software Inc., and is not to be reproduced,
 * transmitted, or disclosed in any way without written permission.
 *
 * Produced by:
 *      Mykola Konyk
 *      Side Effects Software Inc
 *      123 Front Street West, Suite 1401
 *      Toronto, Ontario
 *      Canada   M5J 2M2
 *      416-504-9876
 *
 */

#include "HoudiniEnginePrivatePCH.h"


UHoudiniAssetParameterToggle::UHoudiniAssetParameterToggle(const FPostConstructInitializeProperties& PCIP) :
	Super(PCIP)
{
	// Parameter will have at least one value.
	Values.AddZeroed(1);
}


UHoudiniAssetParameterToggle::~UHoudiniAssetParameterToggle()
{

}


UHoudiniAssetParameterToggle*
UHoudiniAssetParameterToggle::Create(UHoudiniAssetComponent* InHoudiniAssetComponent, HAPI_NodeId InNodeId, const HAPI_ParmInfo& ParmInfo)
{
	UHoudiniAssetParameterToggle* HoudiniAssetParameterToggle = new UHoudiniAssetParameterToggle(FPostConstructInitializeProperties());
	HoudiniAssetParameterToggle->CreateParameter(InHoudiniAssetComponent, InNodeId, ParmInfo);
	return HoudiniAssetParameterToggle;
}


bool
UHoudiniAssetParameterToggle::CreateParameter(UHoudiniAssetComponent* InHoudiniAssetComponent, HAPI_NodeId InNodeId, const HAPI_ParmInfo& ParmInfo)
{
	if(!Super::CreateParameter(InHoudiniAssetComponent, InNodeId, ParmInfo))
	{
		return false;
	}

	// We can only handle toggle type.
	if(HAPI_PARMTYPE_TOGGLE != ParmInfo.type)
	{
		return false;
	}

	// Assign internal Hapi values index.
	SetValuesIndex(ParmInfo.intValuesIndex);

	// Get the actual value for this property.
	Values.SetNumZeroed(TupleSize);
	if(HAPI_RESULT_SUCCESS != HAPI_GetParmIntValues(InNodeId, &Values[0], ValuesIndex, TupleSize))
	{
		return false;
	}

	// Min and max make no sense for this type of parameter.
	return true;
}


void
UHoudiniAssetParameterToggle::CreateWidget(IDetailCategoryBuilder& DetailCategoryBuilder)
{
	Super::CreateWidget(DetailCategoryBuilder);

	FDetailWidgetRow& Row = DetailCategoryBuilder.AddCustomRow(TEXT(""));

	Row.NameWidget.Widget = SNew(STextBlock)
							.Text(FString(""))
							.ToolTipText(Label)
							.Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));

	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	for(int32 Idx = 0; Idx < TupleSize; ++Idx)
	{
		VerticalBox->AddSlot().Padding(0, 2)
		[
			SNew(SCheckBox)
				.OnCheckStateChanged(FOnCheckStateChanged::CreateUObject(this, &UHoudiniAssetParameterToggle::CheckStateChanged, Idx))
				.IsChecked(TAttribute<ESlateCheckBoxState::Type>::Create(TAttribute<ESlateCheckBoxState::Type>::FGetter::CreateUObject(this, &UHoudiniAssetParameterToggle::IsChecked, Idx)))
				.Content()
				[
					SNew(STextBlock)
					.Text(Label)
					.ToolTipText(Label)
					.Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
				]
		];
	}

	Row.ValueWidget.Widget = VerticalBox;
}


bool
UHoudiniAssetParameterToggle::UploadParameterValue()
{
	if(HAPI_RESULT_SUCCESS != HAPI_SetParmIntValues(NodeId, &Values[0], ValuesIndex, TupleSize))
	{
		return false;
	}

	return Super::UploadParameterValue();
}


void
UHoudiniAssetParameterToggle::Serialize(FArchive& Ar)
{
	// Call base implementation.
	Super::Serialize(Ar);
}


void
UHoudiniAssetParameterToggle::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	// Call base implementation.
	Super::AddReferencedObjects(InThis, Collector);
}


void
UHoudiniAssetParameterToggle::CheckStateChanged(ESlateCheckBoxState::Type NewState, int32 Idx)
{
	Values[Idx] = (ESlateCheckBoxState::Checked == NewState);

	// Mark this parameter as changed.
	MarkChanged();
}


ESlateCheckBoxState::Type
UHoudiniAssetParameterToggle::IsChecked(int32 Idx) const
{
	if(Values[Idx])
	{
		return ESlateCheckBoxState::Checked;
	}
	
	return ESlateCheckBoxState::Unchecked;
}
