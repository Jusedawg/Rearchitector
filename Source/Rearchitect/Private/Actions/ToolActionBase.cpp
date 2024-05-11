#include "Actions/ToolActionBase.h"

void UToolActionBase::UndoAction_Implementation()
{
	TArray<FTargetModifyData> TransformDatas;

	for (auto& Cache : UndoCache)
	{
		auto& CacheData = Cache.Value.Transform;
		FActorTransformModifyData Data;
		Data.Move = FPositionModifier(CacheData.GetLocation(), true);
		Data.Rotate = FRotationModifier(CacheData.GetRotation(), true);
		Data.Scale = FScaleModifier(CacheData.GetScale3D(), true);

		TransformDatas.Add(FTargetModifyData(Cache.Key, Data));
	}

	GetRCO()->ApplyModifyDataOnEach(TransformDatas);
}
