modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);

		actions.Insert(eAIActionWeaponControl);
		actions.Insert(eAIActionWeaponUnjam);
		actions.Insert(eAIActionOpenDoorsDummy);
		actions.Insert(eAIActionCloseDoorsDummy);

	}
};
