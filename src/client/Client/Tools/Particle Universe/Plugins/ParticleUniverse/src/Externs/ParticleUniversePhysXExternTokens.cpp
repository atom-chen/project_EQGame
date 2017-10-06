/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef PARTICLE_UNIVERSE_EXPORTS
#define PARTICLE_UNIVERSE_EXPORTS
#endif

#include "ParticleUniversePrerequisites.h"
#ifdef PU_PHYSICS_PHYSX
	#include "Externs/ParticleUniversePhysXExternTokens.h"
	#include "ParticleUniverseScriptDeserializer.h"
	#include "ParticleUniverseScriptSerializer.h"
	#include "Externs/ParticleUniversePhysXExtern.h"

	namespace ParticleUniverse
	{
		// Constants
		const Ogre::String PhysXExternTokens::KEYWORD_SHAPE_TYPE = "physx_shape";
		const Ogre::String PhysXExternTokens::KEYWORD_ACTOR_COLLISION_GROUP = "physx_actor_group";
		const Ogre::String PhysXExternTokens::KEYWORD_SHAPE_COLLISION_GROUP = "physx_shape_group";
		const Ogre::String PhysXExternTokens::KEYWORD_GROUP_MASK = "physx_group_mask";
		const Ogre::String PhysXExternTokens::KEYWORD_ANGULAR_VELOCITY = "physx_angular_velocity";
		const Ogre::String PhysXExternTokens::KEYWORD_ANGULAR_DAMPING = "physx_angular_damping";
		const Ogre::String PhysXExternTokens::KEYWORD_MATERIAL_INDEX = "physx_material_index";
		const Ogre::String PhysXExternTokens::KEYWORD_BOX = "Box";
		const Ogre::String PhysXExternTokens::KEYWORD_SPHERE = "Sphere";
		const Ogre::String PhysXExternTokens::KEYWORD_CAPSULE = "Capsule";
	
		//-----------------------------------------------------------------------
		void PhysXExternTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
		{
			tokenRegister->addLexemeAction(KEYWORD_GROUP_MASK, &mGroupMaskToken);
			tokenRegister->addLexemeAction(KEYWORD_ACTOR_COLLISION_GROUP, &mActorCollisionGroupToken);
			tokenRegister->addLexemeAction(KEYWORD_SHAPE_COLLISION_GROUP, &mShapeCollisionGroupToken);
			tokenRegister->addLexemeAction(KEYWORD_SHAPE_TYPE, &mShapeTypeToken);
			tokenRegister->addLexemeAction(KEYWORD_ANGULAR_VELOCITY, &mAngularVelociyToken);
			tokenRegister->addLexemeAction(KEYWORD_ANGULAR_DAMPING, &mAngularDampingToken);
			tokenRegister->addLexemeAction(KEYWORD_MATERIAL_INDEX, &mMaterialIndexToken);
		}
		//-----------------------------------------------------------------------
		void PhysXExternTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
		{
			// Cast the element to a PhysXExtern
			const Extern* externObject = static_cast<const Extern*>(element);
			const PhysXExtern* physXExtern = static_cast<const PhysXExtern*>(externObject);
			serializer->writeLine(KEYWORD_EXTERN, externObject->getExternType(), externObject->getName(), 8);
			serializer->writeLine("{", 8);
	
			// Write base attributes
			ExternTokens::write(serializer, element);
	
			// Write own attributes
			if (physXExtern->getActorGroup() != 0) serializer->writeLine(
				KEYWORD_ACTOR_COLLISION_GROUP, Ogre::StringConverter::toString(physXExtern->getActorGroup()), 12);
			PhysicsShapeDesc* physicsShapeDesc = physXExtern->mPhysicsShapeDesc;
			if (physicsShapeDesc)
			{
				switch (physicsShapeDesc->mPhysicsShapeType)
				{
					case ST_BOX:
					{
						serializer->writeLine(KEYWORD_SHAPE_TYPE, KEYWORD_BOX, 12);
					}
					break;

					case ST_SPHERE:
					{
						serializer->writeLine(KEYWORD_SHAPE_TYPE, KEYWORD_SPHERE, 12);
					}
					break;

					case ST_CAPSULE:
					{
						serializer->writeLine(KEYWORD_SHAPE_TYPE, KEYWORD_CAPSULE, 12);
					}
					break;
				}

				serializer->writeLine("{", 12);
				if (physicsShapeDesc->mCollisionGroup != 0) serializer->writeLine(
					KEYWORD_SHAPE_COLLISION_GROUP, Ogre::StringConverter::toString(physicsShapeDesc->mCollisionGroup), 16);
				if (physicsShapeDesc->mAngularVelocity != Ogre::Vector3::ZERO) serializer->writeLine(
					KEYWORD_ANGULAR_VELOCITY, Ogre::StringConverter::toString(physicsShapeDesc->mAngularVelocity), 16);
				if (physicsShapeDesc->mAngularDamping != 0.5f) serializer->writeLine(
					KEYWORD_ANGULAR_VELOCITY, Ogre::StringConverter::toString(physicsShapeDesc->mAngularDamping), 16);
				if (physicsShapeDesc->mMaterialIndex != 0) serializer->writeLine(
					KEYWORD_MATERIAL_INDEX, Ogre::StringConverter::toString(physicsShapeDesc->mMaterialIndex), 16);
				if (physicsShapeDesc->mGroupMask != GroupsMask(0, 0, 0, 0)) serializer->writeLine(
					KEYWORD_GROUP_MASK, 
					Ogre::StringConverter::toString(physicsShapeDesc->mGroupMask.bits3) + " " +
					Ogre::StringConverter::toString(physicsShapeDesc->mGroupMask.bits2) + " " +
					Ogre::StringConverter::toString(physicsShapeDesc->mGroupMask.bits1) + " " +
					Ogre::StringConverter::toString(physicsShapeDesc->mGroupMask.bits0), 16);
	
				// Write the close bracket
				serializer->writeLine("}", 12);
			}
	
			// Write the close bracket
			serializer->writeLine("}", 8);
		}
		//-----------------------------------------------------------------------
		void PhysXExternTokens::ShapeTypeToken::parse(ParticleScriptDeserializer* deserializer)
		{
			deserializer->context.validateCurrentSectionName(EXTERN, "PhysXExternTokens::ShapeTypeToken::parse");
			PhysXExtern* physXExtern = static_cast<PhysXExtern*>(deserializer->context.getCurrentSectionElement());
			PhysicsShapeDesc* physicsShapeDesc = physXExtern->mPhysicsShapeDesc;
			Ogre::String type = deserializer->getNextTokenLabel();
			if (type == KEYWORD_BOX)
			{
				physicsShapeDesc->mPhysicsShapeType = ST_BOX;
			}
			else if (type == KEYWORD_SPHERE)
			{
				physicsShapeDesc->mPhysicsShapeType = ST_SPHERE;
			}
			else if (type == KEYWORD_CAPSULE)
			{
				physicsShapeDesc->mPhysicsShapeType = ST_CAPSULE;
			}
			deserializer->context.beginSection(INNER_CONTEXT, physXExtern);
		}
		//-----------------------------------------------------------------------
		void PhysXExternTokens::ActorCollisionGroupToken::parse(ParticleScriptDeserializer* deserializer)
		{
			deserializer->context.validateCurrentSectionName(EXTERN, "PhysXExternTokens::ActorCollisionGroupToken::parse");
			PhysXExtern* physXExtern = static_cast<PhysXExtern*>(deserializer->context.getCurrentSectionElement());
			Ogre::uint16 collisionGroup;
			deserializer->parseUint16(collisionGroup);
			physXExtern->setActorGroup(collisionGroup);
		}
		//-----------------------------------------------------------------------
		void PhysXExternTokens::ShapeCollisionGroupToken::parse(ParticleScriptDeserializer* deserializer)
		{
			deserializer->context.validateCurrentSectionName(INNER_CONTEXT, "PhysXExternTokens::ShapeCollisionGroupToken::parse");
			PhysXExtern* physXExtern = static_cast<PhysXExtern*>(deserializer->context.getCurrentSectionElement());
			PhysicsShapeDesc* physicsShapeDesc = physXExtern->mPhysicsShapeDesc;
			Ogre::uint16 collisionGroup;
			deserializer->parseUint16(collisionGroup);
			physicsShapeDesc->mCollisionGroup = collisionGroup;
		}
		//-----------------------------------------------------------------------
		void PhysXExternTokens::GroupMaskToken::parse(ParticleScriptDeserializer* deserializer)
		{
			deserializer->context.validateCurrentSectionName(INNER_CONTEXT, "PhysXExternTokens::GroupMaskToken::parse");
			PhysXExtern* physXExtern = static_cast<PhysXExtern*>(deserializer->context.getCurrentSectionElement());
			PhysicsShapeDesc* physicsShapeDesc = physXExtern->mPhysicsShapeDesc;
			GroupsMask groupMask;
			deserializer->parse4XUint32(groupMask.bits3, groupMask.bits2, groupMask.bits1, groupMask.bits0);
			physicsShapeDesc->mGroupMask = groupMask;
		}
		//-----------------------------------------------------------------------
		void PhysXExternTokens::AngularVelociyToken::parse(ParticleScriptDeserializer* deserializer)
		{
			deserializer->context.validateCurrentSectionName(INNER_CONTEXT, "PhysXExternTokens::AngularVelociyToken::parse");
			PhysXExtern* physXExtern = static_cast<PhysXExtern*>(deserializer->context.getCurrentSectionElement());
			PhysicsShapeDesc* physicsShapeDesc = physXExtern->mPhysicsShapeDesc;
			Ogre::Vector3 angularVelocity;
			deserializer->parseVector3(angularVelocity);
			physicsShapeDesc->mAngularVelocity = angularVelocity;
		}
		//-----------------------------------------------------------------------
		void PhysXExternTokens::AngularDampingToken::parse(ParticleScriptDeserializer* deserializer)
		{
			deserializer->context.validateCurrentSectionName(INNER_CONTEXT, "PhysXExternTokens::AngularDampingToken::parse");
			PhysXExtern* physXExtern = static_cast<PhysXExtern*>(deserializer->context.getCurrentSectionElement());
			PhysicsShapeDesc* physicsShapeDesc = physXExtern->mPhysicsShapeDesc;
			Ogre::Real angularDamping;
			deserializer->parseReal(angularDamping);
			physicsShapeDesc->mAngularDamping = angularDamping;
		}
		//-----------------------------------------------------------------------
		void PhysXExternTokens::MaterialIndexToken::parse(ParticleScriptDeserializer* deserializer)
		{
			deserializer->context.validateCurrentSectionName(INNER_CONTEXT, "PhysXExternTokens::MaterialIndexToken::parse");
			PhysXExtern* physXExtern = static_cast<PhysXExtern*>(deserializer->context.getCurrentSectionElement());
			PhysicsShapeDesc* physicsShapeDesc = physXExtern->mPhysicsShapeDesc;
			Ogre::uint16 materialIndex;
			deserializer->parseUint16(materialIndex);
			physicsShapeDesc->mMaterialIndex = materialIndex;
		}	

}
#endif //PU_PHYSICS_PHYSX