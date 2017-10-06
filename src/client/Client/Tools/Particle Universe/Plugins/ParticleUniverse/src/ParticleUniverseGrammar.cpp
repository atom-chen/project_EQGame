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

#include "ParticleUniverseGrammar.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
    // Static definitions
    //-----------------------------------------------------------------------

	// ----------------------------------- Begin Main Header -----------------------------------
	Ogre::String Grammar::particleUniverseBeginGrammar = 

	// Top level rule
	"<Script> ::= {<ScriptProperties>} \n"
	"<ScriptProperties> ::= <Alias> | <ParticleSystem> \n"

	// Alias section rules
	"<Alias> ::= 'alias' <AliasName> '{' {<ParticleTechnique>} | {<ParticleRenderer>} | {<ParticleEmitter>} | \n"
	"                                    {<ParticleAffector>} | {<ParticleObserver>} | {<ParticleEventHandler>} | \n"
	"                                    {<Behaviour>} | {<Extern>} '}' \n"

	// Use Alias section rules
	"<UseAlias> ::= 'use_alias' <AliasName> \n"
	"<AliasName> ::= <Label> \n"
		
	// Particle System section rules
	"<ParticleSystem> ::= 'system' <SystemName> '{' {<ParticleSystemProperties>} '}' \n"
	"<ParticleSystemProperties> ::= <ParticleTechnique> | <LodDistances> | <SmoothLod> | <UseAlias> | \n"
	"                               <IterationInterval> | <FixedTimeout> | <NonVisibleUpdateTimeout> | <FastForward> | \n"
	"                               <MainCameraName> | <ScaleVelocity> | <ScaleTime> | <Scale> | <KeepLocal> | <TightBoundingBox> \n"
	"<SystemName> ::= <Label> \n"
	"<LodDistances> ::= 'lod_distances' <#lodDistance> {<#lodDistance>} \n"
	"<SmoothLod> ::= 'smooth_lod' <True_False> \n"
	"<IterationInterval> ::= 'iteration_interval' <#iterationInterval> \n"
	"<FixedTimeout> ::= 'fixed_timeout' <#fixedTimeout> \n"
	"<NonVisibleUpdateTimeout> ::= 'nonvisible_update_timeout' <#nonVisuableUpdateTimeout> \n"
	"<FastForward> ::= 'fast_forward' <#time> <#interval> \n"
	"<MainCameraName> ::= 'main_camera_name' <Label> \n"
	"<ScaleVelocity> ::= 'scale_velocity' <#scaleVelocity> \n"
	"<ScaleTime> ::= 'scale_time' <#scaleTime> \n"
	"<Scale> ::= 'scale' <Vector3> \n"
	"<TightBoundingBox> ::= 'tight_bounding_box' <True_False> \n"
	
	// ----------------------------------- Technique -----------------------------------
	// Particle Technique section rules
	"<ParticleTechnique> ::= 'technique' [<TechniqueName>] '{' {<ParticleTechniqueProperties>} '}' \n"
	"<TechniqueName> ::= <Label> \n"
	"<ParticleTechniqueProperties> ::= <ParticleRenderer> | <ParticleEmitter> | <ParticleAffector> | <ParticleObserver> | \n"
	"                                  <VisualParticleQuota> | <EmittedEmitterQuota> | <EmittedTechniqueQuota> | \n"
	"                                  <EmittedAffectorQuota> | <EmittedSystemQuota> | <Enabled> | <Material> | <LodIndex> | <UseAlias> | \n"
	"                                  <DefaultParticleWidth> | <DefaultParticleHeight> | <DefaultParticleDepth> | \n"
	"                                  <Position> | <KeepLocal> | <Extern> | <Behaviour> | <SpatialHashingCellDimension> | \n"
	"                                  <SpatialHashingCellOverlap> | <SpatialHashTableSize> | <SpatialHashingUpdateInterval> | <MaxVelocity> \n"
	"<VisualParticleQuota> ::= 'visual_particle_quota' <#quota> \n"
	"<EmittedEmitterQuota> ::= 'emitted_emitter_quota' <#quota> \n"
	"<EmittedTechniqueQuota> ::= 'emitted_technique_quota' <#quota> \n"
	"<EmittedAffectorQuota> ::= 'emitted_affector_quota' <#quota> \n"
	"<EmittedSystemQuota> ::= 'emitted_system_quota' <#quota> \n"
	"<Material> ::= 'material' <MaterialName> \n"
	"<MaterialName> ::= <Label> \n"
	"<LodIndex> ::= 'lod_index' <#lodIndex> \n"
	"<DefaultParticleWidth> ::= 'default_particle_width' <#width> [<StreamDependency>] \n"
	"<DefaultParticleHeight> ::= 'default_particle_height' <#height> [<StreamDependency>] \n"
	"<DefaultParticleDepth> ::= 'default_particle_depth' <#depth> [<StreamDependency>] \n"
	"<SpatialHashingCellDimension> ::= 'spatial_hashing_cell_dimension' <#spatialHashingCellDimension> \n"
	"<SpatialHashingCellOverlap> ::= 'spatial_hashing_cell_overlap' <#spatialHashingCellOverlap> \n"
	"<SpatialHashTableSize> ::= 'spatial_hashtable_size' <#spatialHashTableSize> \n"
	"<SpatialHashingUpdateInterval> ::= 'spatial_hashing_update_interval' <#spatialHashingUpdateInterval> \n"
	"<MaxVelocity> ::= 'max_velocity' <#maxVelocity> \n"
	;
	// ----------------------------------- End Main Header -----------------------------------

	// ----------------------------------- Begin Renderer -----------------------------------
	// Particle Renderer section rules
	Ogre::String Grammar::particleRendererHeaderGrammar = 
	"<ParticleRenderer> ::= 'renderer' <RendererType> '{' {<ParticleRendererProperties>} '}' \n"
	;

	Ogre::String Grammar::particleRendererPropertiesGrammar = 
	"<ParticleRendererProperties> ::= <BillboardRendererProperties> | <EntityRendererProperties> | <RibbonTrailRendererProperties> | <LightRendererProperties> | \n"
	"                                 <RenderQueueGroup> | <Sorting> | <TextureCoordsSet> | <TextureCoordsRows> | <TextureCoordsColumns> | \n"
	"                                 <UseSoftParticles> | <SoftParticlesContrastPower> | <SoftParticlesScale> | <SoftParticlesDelta> \n"
	"<Sorting> ::= 'sorting' <True_False> \n"
	"<RenderQueueGroup> ::= 'render_queue_group' <#rqg> \n"
    "<TextureCoordsSet> ::= 'texture_coords_set' '{' {<DefineTextureCoords>} '}' \n"
	"<TextureCoordsRows> ::= 'texture_coords_rows' <#textureCoordsRows> \n"
	"<DefineTextureCoords> ::= 'texture_coords_define' <Vector4> \n"
	"<TextureCoordsColumns> ::= 'texture_coords_columns' <#textureCoordsColumns> \n"
	"<UseSoftParticles> ::= 'use_soft_particles' <True_False> \n"
	"<SoftParticlesContrastPower> ::= 'soft_particles_contrast_power' <#softParticlesContrastPower> \n"
	"<SoftParticlesScale> ::= 'soft_particles_scale' <#softParticlesScale> \n"
	"<SoftParticlesDelta> ::= 'soft_particles_delta' <#softParticlesDelta> \n"
	;

	Ogre::String Grammar::particleRendererDetailedPropertiesGrammar = 
	"<BillboardRendererProperties> ::= <BillboardTypeProperties> | <BillboardOriginProperties> | <BillboardRotationTypeProperties> | \n"
	"                                  <BbtCommonDirection> | <BbtCommonUpVector> | <BbtPointRendering> | \n"
	"                                  <BbtAccurateFacing> \n"
	"<EntityRendererProperties> ::= <EntityRendererMeshName> | <EntityOrientationType> \n"
	"<RibbonTrailRendererProperties> ::= <RibbonTrailVertexColours> | <RibbonTrailMaxElements> | <RibbonTrailLength> |  <RibbonTrailWidth> | \n"
	"                                    <RibbonTrailRandomInitialColours> | <RibbonTrailInitialColours> | <RibbonTrailColourChange> \n"
	"<LightRendererProperties> ::= <LightRendererLightType> | <LightRendererDiffuseColour> | <LightRendererSpecularColour> |  \n"
	"                              <LightRendererAttRange> | <LightRendererAttConstant> | <LightRendererAttLinear> | <LightRendererAttQuadratic> |  \n"
	"                              <LightRendererSpotInner> | <LightRendererSpotOuter> | <LightRendererFalloff> | <LightRendererPowerScale> \n"
	"<BillboardTypeProperties> ::= 'billboard_type' {<BillboardType>} \n"
	"<BillboardOriginProperties> ::= 'billboard_origin' {<BillboardOrigin>} \n"
	"<BillboardRotationTypeProperties> ::= 'billboard_rotation_type' {<BillboardRotation>} \n"
	"<BillboardType> ::= <Point> | <BbtOrientedCommon> | <BbtOrientedSelf> | \n"
	"                    <BbtOrientedShape> | <BbtPerpendicularCommon> | <BbtPerpendicularSelf> \n"
	"<BillboardOrigin> ::= <BbtOriginTopLeft> | <BbtOriginTopCenter> | \n"
	"                      <BbtOriginTopRight> | <BbtOriginCenterLeft> | \n"
	"                      <BbtOriginCenter> | <BbtOriginCenterRight> | \n"
	"                      <BbtOriginBottomLeft> | <BbtOriginBottomCenter> | \n"
	"                      <BbtOriginBottomRight> \n"
	"<BillboardRotation> ::= <BbtRotationTypeVertex> | <BbtRotationTypeTexCoord> \n"	
	"<BbtOrientedCommon> ::= 'oriented_common' \n"
	"<BbtOrientedSelf> ::= 'oriented_self' \n"
	"<BbtOrientedShape> ::= 'oriented_shape' \n"
	"<BbtPerpendicularCommon> ::= 'perpendicular_common' \n"
	"<BbtPerpendicularSelf> ::= 'perpendicular_self' \n"
	"<BbtOriginTopLeft> ::= 'top_left' \n"
	"<BbtOriginTopCenter> ::= 'top_center' \n"
	"<BbtOriginTopRight> ::= 'top_right' \n"
	"<BbtOriginCenterLeft> ::= 'center_left' \n"
	"<BbtOriginCenterRight> ::= 'center_right' \n"
	"<BbtOriginCenter> ::= 'center' \n"
	"<BbtOriginBottomLeft> ::= 'bottom_left' \n"
	"<BbtOriginBottomCenter> ::= 'bottom_center' \n"
	"<BbtOriginBottomRight> ::= 'bottom_right' \n"
	"<BbtRotationTypeVertex> ::= 'vertex' \n"
	"<BbtRotationTypeTexCoord> ::= 'texcoord' \n"
	"<BbtCommonDirection> ::= 'common_direction' <Vector3> \n"
	"<BbtCommonUpVector> ::= 'common_up_vector' <Vector3> \n"
	"<BbtPointRendering> ::= 'point_rendering' <True_False> \n"
	"<BbtAccurateFacing> ::= 'accurate_facing' <On_Off> \n"
	"<RendererType> ::= <Label> \n"
	"<EntityRendererMeshName> ::= 'entity_renderer_mesh_name' <Label> \n"
	"<EntityOrientationType> ::= 'entity_orientation_type' <Label> \n"
	"<RibbonTrailVertexColours> ::= 'ribbontrail_vertex_colours' <True_False> \n"
	"<RibbonTrailMaxElements> ::= 'ribbontrail_max_elements' <#MaxElements> \n"
	"<RibbonTrailLength> ::= 'ribbontrail_length' <#TrailLength> \n"
	"<RibbonTrailWidth> ::= 'ribbontrail_width' <#TrailWidth> \n"
	"<RibbonTrailRandomInitialColours> ::= 'ribbontrail_random_initial_colour' <True_False> \n"
	"<RibbonTrailInitialColours> ::= 'ribbontrail_initial_colour' <Colour> \n"
	"<RibbonTrailColourChange> ::= 'ribbontrail_colour_change' <Colour> \n"
	"<LightRendererLightType> ::= 'light_renderer_light_type' <Label> \n"
	"<LightRendererDiffuseColour> ::= 'light_renderer_diffuse' <Colour> \n"
	"<LightRendererSpecularColour> ::= 'light_renderer_specular' <Colour> \n"
	"<LightRendererAttRange> ::= 'light_renderer_att_range' <#LightRendererAttRange> \n"
	"<LightRendererAttConstant> ::= 'light_renderer_att_constant' <#LightRendererAttConstant> \n"
	"<LightRendererAttLinear> ::= 'light_renderer_att_linear' <#LightRendererAttLinear> \n"
	"<LightRendererAttQuadratic> ::= 'light_renderer_att_quadratic' <#LightRendererAttQuadratic> \n"
	"<LightRendererSpotInner> ::= 'light_renderer_spot_inner' <#LightRendererSpotInner> \n"
	"<LightRendererSpotOuter> ::= 'light_renderer_spot_outer' <#LightRendererSpotOuter> \n"
	"<LightRendererFalloff> ::= 'light_renderer_falloff' <#LightRendererFalloff> \n"
	"<LightRendererPowerScale> ::= 'light_renderer_powerscale' <#LightRendererPowerScale> \n"
	;
	// ----------------------------------- End Renderer -----------------------------------


	// ----------------------------------- Begin Emitter -----------------------------------
	Ogre::String Grammar::particleEmitterHeaderGrammar = 
	"<ParticleEmitter> ::= 'emitter' <EmitterType> [<EmitterName>] '{' {<ParticleEmitterProperties>} '}' \n"
	;

	Ogre::String Grammar::particleEmitterPropertiesGrammar = 
	"<ParticleEmitterProperties> ::= <ParticleEmitterBaseProperties> | \n"
	"                                <LineEmitterProperties> | <BoxEmitterProperties> | <SphereEmitterProperties> | \n"
	"                                <VertexEmitterProperties> | <CircleEmitterProperties> | <MeshSurfaceEmitterProperties> | \n"
	"                                <PositionEmitterProperties> | <SlaveEmitterProperties> \n"
	;

	Ogre::String Grammar::particleEmitterDetailedPropertiesGrammar = 
	"<ParticleEmitterBaseProperties> ::= <Angle> | <Position> | <Direction> | <Orientation> | <OrientationRangeStart> |<OrientationRangeEnd> | \n"
	"                                    <Emits> | <EmissionRate> | <TimeToLive> | <Mass> | <Velocity> | <Duration> | <RepeatDelay> | <KeepLocal> | \n"
	"                                    <AllParticleDimensions> | <ParticleWidth> | <ParticleHeight> | <ParticleDepth> | <Enabled> | \n"
	"                                    <AutoDirection> | <ForceEmission> | <TextureCoordsRangeStart> | <TextureCoordsRangeEnd> | <TextureCoords>  | \n"
	"                                    <ParticleColour> | <ColourRangeStart> | <ColourRangeEnd> \n"
	"<LineEmitterProperties> ::= <LineEmEnd> | <LineEmMaxIncrement> | <LineEmMinIncrement> | <LineEmMaxDeviation> \n"
	"<LineEmEnd> ::= 'line_em_end' <Vector3> \n"
	"<LineEmMaxIncrement> ::= 'line_em_max_increment' <#lineEmMaxIncrement> \n"
	"<LineEmMinIncrement> ::= 'line_em_min_increment' <#lineEmMinIncrement> \n"
	"<LineEmMaxDeviation> ::= 'line_em_max_deviation' <#lineEmMaxDeviation> \n"
	"<BoxEmitterProperties> ::= <BoxEmHeigth> | <BoxEmWidth> | <BoxEmDepth> \n"
	"<SphereEmitterProperties> ::= <SphereSurfaceEmRadius> \n"
	"<VertexEmitterProperties> ::= <VertexEmStep> | <VertexEmSegments> | <VertexEmIterations> | <VertexEmMeshName> \n"
	"<CircleEmitterProperties> ::= <CircleEmRadius> | <CircleEmAngle> | <CircleEmStep> | <CircleEmRandom> | <CircleEmNormal> \n"
	"<MeshSurfaceEmitterProperties> ::= <MeshSurfaceEmMeshName> | <MeshSurfaceEmDistribution> | <MeshSurfaceEmScale> \n"
	"<PositionEmitterProperties> ::= <PositionAdd> | <PositionRandom> \n"
	"<SlaveEmitterProperties> ::= <MasterTechniqueName> | <MasterEmitterName> \n"
	"<Angle> ::= 'angle' <DynamicAttribute> \n"
	"<Direction> ::= 'direction' <Vector3> \n"
	"<Orientation> ::= 'orientation' <Quaternion> \n"
	"<OrientationRangeStart> ::= 'range_start_orientation' <Quaternion> \n"
	"<OrientationRangeEnd> ::= 'range_end_orientation' <Quaternion> \n"
	"<EmissionRate> ::= 'emission_rate' <DynamicAttribute> [<StreamDependency>] \n"
	"<TimeToLive> ::= 'time_to_live' <DynamicAttribute> \n"
	"<Emits> ::= 'emits' {<EmitsParticle>} | {<EmitsTechnique>} | {<EmitsEmitter>} | {<EmitsAffector>} | {<EmitsSystem>} \n"
	"<EmitsParticle> ::= <VisualParticle> \n"
	"<EmitsTechnique> ::= <TechniqueParticle> <TechniqueName> \n"
	"<EmitsEmitter> ::= <EmitterParticle> <EmitterName> \n"
	"<EmitsAffector> ::= <AffectorParticle> <AffectorName> \n"
	"<EmitsSystem> ::= <SystemParticle> <SystemName> \n"
	"<Mass> ::= 'mass' <DynamicAttribute> \n"
	"<Velocity> ::= 'velocity' <DynamicAttribute> \n"
	"<Duration> ::= 'duration' <DynamicAttribute> \n"
	"<RepeatDelay> ::= 'repeat_delay' <DynamicAttribute> \n"
	"<AllParticleDimensions> ::= 'all_particle_dimensions' <DynamicAttribute> \n"
	"<ParticleWidth> ::= 'particle_width' <DynamicAttribute> \n"
	"<ParticleHeight> ::= 'particle_height' <DynamicAttribute> \n"
	"<ParticleDepth> ::= 'particle_depth' <DynamicAttribute> \n"
	"<AutoDirection> ::= 'auto_direction' <True_False> \n"
	"<ForceEmission> ::= 'force_emission' <True_False> \n"
	"<TextureCoordsRangeStart> ::= 'start_texture_coords' <#textureCoordsRangeStart> \n"
	"<TextureCoordsRangeEnd> ::= 'end_texture_coords' <#textureCoordsRangeEnd> \n"
	"<TextureCoords> ::= 'texture_coords' <#textureCoords> \n"
	"<ColourRangeStart> ::= 'start_colour_range' <Colour> \n"
	"<ColourRangeEnd> ::= 'end_colour_range' <Colour> \n"
	"<ParticleColour> ::= 'colour' <Colour> \n"
	"<BoxEmWidth> ::= 'box_em_width' <#boxEmWidth> \n"
	"<BoxEmHeigth> ::= 'box_em_height' <#boxEmHeight> \n"
	"<BoxEmDepth> ::= 'box_em_depth' <#boxEmDepth> \n"
	"<SphereSurfaceEmRadius> ::= 'sphere_surface_em_radius' <#sphereSurfaceEmRadius> \n"
	"<VertexEmStep> ::= 'vertex_em_step' <#vertexEmStep> \n"
	"<VertexEmSegments> ::= 'vertex_em_segments' <#vertexEmSegments> \n"
	"<VertexEmIterations> ::= 'vertex_em_iterations' <#vertexEmIterations> \n"
	"<VertexEmMeshName> ::= 'vertex_em_mesh_name' <Label> \n"
	"<CircleEmRadius> ::= 'circle_em_radius' <#circleEmRadius> \n"
	"<CircleEmAngle> ::= 'circle_em_angle' <#circleEmAngle> \n"
	"<CircleEmStep> ::= 'circle_em_step' <#circleEmStep> \n"
	"<CircleEmRandom> ::= 'circle_em_random' <True_False> \n"
	"<CircleEmNormal> ::= 'circle_em_normal' <Vector3> \n"
	"<MeshSurfaceEmMeshName> ::= 'mesh_surface_mesh_name' <Label> \n"
	"<MeshSurfaceEmDistribution> ::= 'mesh_surface_distribution' <Label> \n"
	"<MeshSurfaceEmScale> ::= 'mesh_surface_scale' <Vector3> \n"
	"<PositionRandom> ::= 'random_position' <True_False> \n"
	"<PositionAdd> ::= 'add_position' <Vector3> \n"
	"<MasterTechniqueName> ::= 'master_technique_name' <Label> \n"
	"<MasterEmitterName> ::= 'master_emitter_name' <Label> \n"
	"<EmitterType> ::= <Label> \n"
	"<EmitterName> ::= <Label> \n"
	"<QuotedEmitterName> ::= <Quoted_Label> \n"
	;
	// ----------------------------------- End Emitter -----------------------------------


	// ----------------------------------- Begin Affector -----------------------------------
	Ogre::String Grammar::particleAffectorHeaderGrammar = 
	"<ParticleAffector> ::= 'affector' <AffectorType> [<AffectorName>] '{' {<ParticleAffectorProperties>} '}' \n"
	;

	Ogre::String Grammar::particleAffectorPropertiesGrammar = 
	"<ParticleAffectorProperties> ::= <ParticleAffectorBaseProperties> | <LinearForceAffectorProperties> | <GravityAffectorProperties> | \n"
	"                                 <ScaleAffectorProperties> | <SineForceAffectorProperties> | <ColourAffectorProperties> | \n"
	"                                 <RandomiserProperties> | <LineAffectorProperties> | <JetAffectorProperties> | \n"
	"                                 <AlignAffectorProperties> | <VortexAffectorProperties> | <GeometryRotatorProperties> | \n"
	"                                 <TextureRotatorProperties> | <TextureAnimatorProperties> | <ParticleFollowerProperties> | <SphereColliderProperties> | \n"
	"                                 <PlaneColliderProperties> | <BoxColliderProperties> | <InterParticleColliderProperties> | <PathFollowerProperties> | \n"
	"                                 <CollisionAvoidanceProperties> | <VelocityMatchingProperties> \n"
	;

	Ogre::String Grammar::particleAffectorDetailedPropertiesGrammar = 
	"<ParticleAffectorBaseProperties> ::= <AffectorMass> | <ExcludeEmitter> | <Position> | <Enabled> | <AffectSpecialisation> \n"
	"<LinearForceAffectorProperties> ::= <ForceVector> | <ForceApplication> \n"
	"<GravityAffectorProperties> ::= <Gravity> \n"
	"<ScaleAffectorProperties> ::= <ScaleAffScaleXYZ> | <ScaleAffScaleX> | <ScaleAffScaleY> | <ScaleAffScaleZ> \n"
	"<ColourAffectorProperties> ::= <TimeAndColour> | <ColourOperation> \n"
	"<RandomiserProperties> ::= <RandAffMaxDeviationX> | <RandAffMaxDeviationY> | <RandAffMaxDeviationZ> | <RandAffDirection> | <RandAffTimeStep> \n"
	"<LineAffectorProperties> ::= <LineAffMaxDeviation> | <LineAffTimeStep> | <LineAffEnd> | <LineAffDrift> \n"
	"<JetAffectorProperties> ::= <JetAcceleration> \n"
	"<AlignAffectorProperties> ::= <AlignResize> \n"
	"<VortexAffectorProperties> ::= <VortexRotationVector> | <VortexRotationSpeed> \n"
	"<GeometryRotatorProperties> ::= <GeomRotUseOwnRotationSpeed> | <GeomRotRotationSpeed> | <GeomRotAxis> \n"
	"<TextureRotatorProperties> ::= <TexRotUseOwnRotationSpeed> | <TexRotRotation> | <TexRotRotationSpeed> \n"
	"<TextureAnimatorProperties> ::= <TexAnimTimeStep> | <TexAnimCoordsStart> | <TexAnimCoordsEnd> | <TexAnimType> | <TexAnimStartRandom> \n"
	"<ParticleFollowerProperties> ::= <FollowerMaxDistance> | <FollowerMinDistance> \n"
	"<SineForceAffectorProperties> ::= <ForceVector> | <ForceApplication> | <SinefAffFrequencyMin> | <SinefAffFrequencyMax> \n"
	"<SphereColliderProperties> ::= <CollisionFriction> | <CollisionBouncyness> | <CollisionIntersectionType> | <CollisionType> | <SphereColliderRadius> \n"
	"<PlaneColliderProperties> ::= <CollisionFriction> | <CollisionBouncyness> | <CollisionIntersectionType> | <PlaneColliderNormal> \n"
	"<BoxColliderProperties> ::= <CollisionFriction> | <CollisionBouncyness> | <CollisionIntersectionType> | <BoxColliderWidth> | <BoxColliderHeight> | \n"
	"                            <BoxColliderDepth> \n"
	"<InterParticleColliderProperties> ::= <InterParticleAdjustment> | <InterParticleCollisionResponse> \n"
	"<PathFollowerProperties> ::= <PathFollowerPoint> \n"
	"<CollisionAvoidanceProperties> ::= <AvoidanceRadius> \n"
	"<VelocityMatchingProperties> ::= <VelocityMatchingRadius> \n"
	"<AffectorMass> ::= 'mass_affector' <#mass> \n"
	"<ExcludeEmitter> ::= 'exclude_emitter' <QuotedEmitterName> {<QuotedEmitterName>} \n"
	"<AffectSpecialisation> ::= 'affect_specialisation' <AffectSpecialisationProperties> \n"
	"<AffectSpecialisationProperties> ::= 'special_default' | 'special_ttl_increase' | 'special_ttl_decrease' \n"
	"<ForceVector> ::= 'force_aff_vector' <Vector3> \n"
	"<ForceApplication> ::= 'force_aff_application' <ForceApplicationSelection> \n"
	"<ForceApplicationSelection> ::= 'add' | 'average' \n"
	"<Gravity> ::= 'gravity' <#Gravity> \n"
	"<ScaleAffScaleXYZ> ::= 'xyz_scale' <DynamicAttribute> \n"
	"<ScaleAffScaleX> ::= 'x_scale' <DynamicAttribute> \n"
	"<ScaleAffScaleY> ::= 'y_scale' <DynamicAttribute> \n"
	"<ScaleAffScaleZ> ::= 'z_scale' <DynamicAttribute> \n"
	"<SinefAffFrequencyMin> ::= 'sinef_aff_frequency_min' <#sinefAffFrequencyMin> \n"
	"<SinefAffFrequencyMax> ::= 'sinef_aff_frequency_max' <#sinefAffFrequencyMax> \n"
	"<TimeAndColour> ::= 'colour_aff_time_colour' <#timeFraction> <Colour> \n"
	"<ColourOperation> ::= 'colour_operation' <Label> \n"
	"<RandAffMaxDeviationX> ::= 'rand_aff_max_deviation_x' <#randAffDeviationX> \n"
	"<RandAffMaxDeviationY> ::= 'rand_aff_max_deviation_y' <#randAffDeviationY> \n"
	"<RandAffMaxDeviationZ> ::= 'rand_aff_max_deviation_z' <#randAffDeviationZ> \n"
	"<RandAffDirection> ::= 'rand_aff_direction' <True_False> \n"
	"<RandAffTimeStep> ::= 'rand_aff_time_step' <#randAffTimeStep> \n"
	"<LineAffMaxDeviation> ::= 'line_aff_max_deviation' <#lineAffMaxDeviation> \n"
	"<LineAffTimeStep> ::= 'line_aff_time_step' <#lineAffTimeStep> \n"
	"<LineAffEnd> ::= 'line_aff_end' <Vector3> \n"
	"<LineAffDrift> ::= 'line_aff_drift' <#lineAffDrift> \n"
	"<JetAcceleration> ::= 'jet_aff_accel' <DynamicAttribute> \n"
	"<AlignResize> ::= 'align_aff_resize' <True_False> \n"
	"<VortexRotationVector> ::= 'vortex_aff_vector' <Vector3> \n"
	"<VortexRotationSpeed> ::= 'vortex_aff_speed' <DynamicAttribute> \n"
	"<GeomRotUseOwnRotationSpeed> ::= 'geom_rot_use_own_rotation' <True_False> \n"
	"<GeomRotRotationSpeed> ::= 'geom_rot_rotation_speed' <DynamicAttribute> \n"
	"<GeomRotAxis> ::= 'geom_rot_axis' <Vector3> \n"
	"<TexRotUseOwnRotationSpeed> ::= 'tex_rot_use_own_rotation' <True_False> \n"
	"<TexRotRotation> ::= 'tex_rot_rotation' <DynamicAttribute> \n"
	"<TexRotRotationSpeed> ::= 'tex_rot_speed' <DynamicAttribute> \n"
	"<TexAnimTimeStep> ::= 'time_step_animation' <#texAnimTimeStep> \n"
	"<TexAnimCoordsStart> ::= 'texture_coords_start' <#texAnimCoordsStart> \n"
	"<TexAnimCoordsEnd> ::= 'texture_coords_end' <#texAnimCoordsEnd> \n"
	"<TexAnimType> ::= 'texture_animation_type' <Label> \n"
	"<TexAnimStartRandom> ::= 'texture_start_random' <True_False> \n"
	"<FollowerMaxDistance> ::= 'follower_max_distance' <#MaxDistance> \n"
	"<FollowerMinDistance> ::= 'follower_min_distance' <#MinDistance> \n"
	"<CollisionFriction> ::= 'collision_friction' <#friction> \n"
	"<CollisionBouncyness> ::= 'collision_bouncyness' <#bouncyness> \n"
	"<CollisionIntersectionType> ::= 'collision_intersection' <Point_Box> \n"
	"<Point_Box > ::= <Point> | <Box> \n"
	"<CollisionType> ::= 'collision_type' <CollisionTypes> \n"
	"<CollisionTypes> ::= 'bounce' | 'flow' | 'none' \n"
	"<SphereColliderRadius> ::= 'sphere_collider_radius' <#SphereColliderRadius> \n"
	"<PlaneColliderNormal> ::= 'plane_collider_normal' <Vector3> \n"
	"<BoxColliderWidth> ::= 'box_collider_width' <#BoxColliderWidth> \n"
	"<BoxColliderHeight> ::= 'box_collider_height' <#BoxColliderHeight> \n"
	"<BoxColliderDepth> ::= 'box_collider_depth' <#BoxColliderDepth> \n"
	"<InterParticleAdjustment> ::= 'ip_adjustment' <#interParticleAdjustment> \n"
	"<InterParticleCollisionResponse> ::= 'ip_collision_response' <Label> \n"
	"<PathFollowerPoint> ::= 'path_follower_point' <Vector3> \n"
	"<AvoidanceRadius> ::= 'avoidance_radius' <#avoidanceRadius> \n"
	"<VelocityMatchingRadius> ::= 'velocity_matching_radius' <#velocityMatchingRadius> \n"
	"<AffectorType> ::= <Label> \n"
	"<AffectorName> ::= <Label> \n"
	;
	// ----------------------------------- End Affector -----------------------------------


	// ----------------------------------- Begin Observer -----------------------------------
	Ogre::String Grammar::particleObserverHeaderGrammar = 
	"<ParticleObserver> ::= 'observer' <ObserverType> [<ObserverName>] '{' {<ObserverProperties>} '}' \n"
	;

	Ogre::String Grammar::particleObserverPropertiesGrammar = 
	"<ObserverProperties> ::= <BaseObserverProperties> | \n"
	"<OnPositionObserverProperties> | <OnCountObserverProperties> | <OnTimeObserverProperties> | \n"
	"<OnVelocityObserverProperties> | <OnEventFlagObserverProperties> | <OnRandomObserverProperties> \n"
	;

	Ogre::String Grammar::particleObserverDetailedPropertiesGrammar = 
	"<BaseObserverProperties> ::= <ObserveParticleType> | <ObserveInterval> | <ObserveUntilEvent> | <ParticleEventHandler> | <Enabled> \n"
	"<ObserveParticleType> ::= 'observe_particle_type' <ParticleTypeValues> \n"
	"<ObserveInterval> ::= 'observe_interval' <#observeInterval> \n"
	"<ObserveUntilEvent> ::= 'observe_until_event' <True_False> \n"
	"<OnPositionObserverProperties> ::= <PositionThresholdX> | <PositionThresholdY> | <PositionThresholdZ> \n"
	"<OnCountObserverProperties> ::= <CountThreshold> \n"
	"<OnVelocityObserverProperties> ::= <VelocityThreshold> \n"
	"<OnEventFlagObserverProperties> ::= <EventFlag> \n"
	"<OnRandomObserverProperties> ::= <RandomThreshold> \n"
	"<PositionThresholdX> ::= 'position_x' <LessGreaterThan> <#xThreshold> \n"
	"<PositionThresholdY> ::= 'position_y' <LessGreaterThan> <#yThreshold> \n"
	"<PositionThresholdZ> ::= 'position_z' <LessGreaterThan> <#zThreshold> \n"
	"<CountThreshold> ::= 'count_threshold' <LessGreaterThan> <#countThreshold> \n"
	"<VelocityThreshold> ::= 'velocity_threshold' <LessGreaterThan> <#velocityThreshold> \n"
	"<EventFlag> ::= 'event_flag' <#eventFlag> \n"
	"<RandomThreshold> ::= 'random_threshold' <#randomThreshold> \n"
	"<OnTimeObserverProperties> ::= <OnTimeThreshold> | <OnTimeSinceStartSystem> \n"
	"<OnTimeThreshold> ::= 'on_time' <LessGreaterThan> <#timeThreshold> \n"
	"<OnTimeSinceStartSystem> ::= 'since_start_system' <True_False> \n"
	"<ObserverType> ::= <Label> \n"
	"<ObserverName> ::= <Label> \n"
	;
	// ----------------------------------- End Observer -----------------------------------


	// ----------------------------------- Begin EventHandler -----------------------------------
	Ogre::String Grammar::particleEventHandlerHeaderGrammar = 
	"<ParticleEventHandler> ::= 'handler' <EventHandlerType> [<EventHandlerName>] '{' {<EventHandlerProperties>} '}' \n"
	;

	Ogre::String Grammar::particleEventHandlerPropertiesGrammar = 
	"<EventHandlerProperties> ::= <DoForceEmissionEventHandlerProperties> | <DoEnableEmitterEventHandlerProperties> | \n"
	"                             <DoEnableComponentEventHandlerProperties> | <DoAffectorEventHandlerProperties> | \n"
	"                             <DoScaleEventHandlerProperties> \n"
	;

	Ogre::String Grammar::particleEventHandlerDetailedPropertiesGrammar = 
	"<DoForceEmissionEventHandlerProperties> ::= <ForceEmitter> | <NumberOfParticles> \n"
	"<DoEnableEmitterEventHandlerProperties> ::= <EnableEmitterName> | <EnableEmitter> \n"
	"<DoEnableComponentEventHandlerProperties> ::= <EnableComponent> \n"
	"<DoAffectorEventHandlerProperties> ::= <DoAffectorName> | <PrePost> \n"
	"<DoScaleEventHandlerProperties> ::= <ScaleFraction> | <ScaleType> \n"
	"<ForceEmitter> ::= 'force_emitter' <EmitterName> \n"
	"<NumberOfParticles> ::= 'number_of_particles' <#numberOfParticles> \n"
	"<EnableEmitterName> ::= 'emitter_to_enable' <Label> \n"
	"<EnableEmitter> ::= 'enable_emitter' <True_False> \n"
	"<EnableComponent> ::= 'enable_component' <ComponentType> <Label> <True_False> \n"
	"<DoAffectorName> ::= 'force_affector' <AffectorName> \n"
	"<PrePost> ::= 'pre_post' <True_False> \n"
	"<ScaleFraction> ::= 'scale_fraction' <#scaleFraction> \n"
	"<ScaleType> ::= 'scale_type' <Label> \n"
	"<EventHandlerType> ::= <Label> \n"
	"<EventHandlerName> ::= <Label> \n"
	;
	// ----------------------------------- End EventHandler -----------------------------------

	// ----------------------------------- Begin Behaviour -----------------------------------
	Ogre::String Grammar::particleBehaviourHeaderGrammar = 
	"<Behaviour> ::= 'behaviour' <BehaviourType> '{' {<BehaviourProperties>} '}' \n"
	;

	Ogre::String Grammar::particleBehaviourPropertiesGrammar = 
	"<BehaviourProperties> ::= <BehaviourBaseProperties> \n"
	;

	Ogre::String Grammar::particleBehaviourDetailedPropertiesGrammar = 
	"<BehaviourBaseProperties> ::= <Test> \n"
	"<BehaviourType> ::= <Label> \n"
	;
	// ----------------------------------- End Behaviour -----------------------------------

	// ----------------------------------- Begin Extern -----------------------------------
	Ogre::String Grammar::externHeaderGrammar = 
	"<Extern> ::= 'extern' <ExternType> [<ExternName>] '{' {<ExternProperties>} '}' \n"
	;

	Ogre::String Grammar::externPropertiesGrammar = 
	"<ExternProperties> ::= <ExternBaseProperties> | <GravityExternProperties> | <SphereColliderExternProperties> | <BoxColliderExternProperties> | \n"
#ifdef PU_PHYSICS_PHYSX
	"                       <PhysXExternProperties> | \n"
#endif //PU_PHYSICS_PHYSX
	"                       <VortexExternProperties> | <SceneDecoratorExternProperties> \n"
	;

	// The distance threshold of the attachable is put here for convenience
	Ogre::String Grammar::externDetailedPropertiesGrammar = 
	"<ExternBaseProperties> ::= <AttachableDistanceThreshold> \n"
	"<GravityExternProperties> ::= <Gravity> \n"
	"<SphereColliderExternProperties> ::= <CollisionFriction> | <CollisionBouncyness> | <CollisionIntersectionType> | <CollisionType> | <SphereColliderRadius> \n"
	"<BoxColliderExternProperties> ::= <CollisionFriction> | <CollisionBouncyness> | <CollisionIntersectionType> | <BoxColliderWidth> | <BoxColliderHeight> | \n"
	"                                  <BoxColliderDepth> \n"
	"<VortexExternProperties> ::= <VortexRotationVector> | <VortexRotationSpeed> | <VortexDistanceThreshold> \n"
	"<VortexDistanceThreshold> ::= 'vortex_distance_threshold' <#vortexDistanceThreshold> \n"
	"<SceneDecoratorExternProperties> ::= <SceneDecoratorMeshName> | <SceneDecoratorMaterialName> | <SceneDecoratorScale> | <SceneDecoratorPosition> \n"
	"<SceneDecoratorMeshName> ::= 'scene_mesh_name' <Label> \n"
	"<SceneDecoratorMaterialName> ::= 'scene_material_name' <Label> \n"
	"<SceneDecoratorScale> ::= 'scene_node_scale' <Vector3> \n"
	"<SceneDecoratorPosition> ::= 'scene_node_position' <Vector3> \n"

#ifdef PU_PHYSICS_PHYSX
	"<PhysXExternProperties> ::= <PhysXActorCollisionGroup> | <PhysXShapeProperties> \n"
	"<PhysXShapeProperties> ::= 'physx_shape' <PhysXShape> '{' {<PhysXShapeDefineProperties>} '}' \n"
	"<PhysXShapeDefineProperties> ::= <PhysXShapeCollisionGroup> | <PhysXGroupMask> | <PhysXAngularVelocity> | <PhysXAngularDamping> | \n"
	"                                 <PhysXMaterialIndex> \n"
	"<PhysXActorCollisionGroup> ::= 'physx_actor_group' <#physXActorCollisionGroup> \n"
	"<PhysXShapeCollisionGroup> ::= 'physx_shape_group' <#physXShapeCollisionGroup> \n"
	"<PhysXGroupMask> ::= 'physx_group_mask' <#bits3> <#bits2> <#bits1> <#bits0> \n"
	"<PhysXAngularVelocity> ::= 'physx_angular_velocity' <Vector3> \n"
	"<PhysXAngularDamping> ::= 'physx_angular_damping' <#physXAngularDamping> \n"
	"<PhysXMaterialIndex> ::= 'physx_material_index' <#physXMaterialIndex> \n"
	"<PhysXShape> ::= <Label> \n"
#endif //PU_PHYSICS_PHYSX

	// Attachable
	"<AttachableDistanceThreshold> ::= 'attachable_distance_threshold' <#attachableDistanceThreshold> \n"

	"<ExternType> ::= <Label> \n"
	"<ExternName> ::= <Label> \n"
	;
	// ----------------------------------- End Extern -----------------------------------


	// ----------------------------------- Begin Main Footer -----------------------------------
	Ogre::String Grammar::particleUniverseEndGrammar = 
	
	// Dynamic Attribute section rules
	"<DynamicAttribute> ::= {<DynamicAttributeFixed>} | {<DynamicAttributeRandom>} | {<DynamicAttributeCurvedLinear>} | {<DynamicAttributeCurvedSpline>} | {<DynamicAttributeOscillate>} \n"
	"<DynamicAttributeFixed> ::= <#fixed> \n"
	"<DynamicAttributeRandom> ::= 'dyn_random' '{' {<DynamicAttributeRandomProperties>} '}' \n"
	"<DynamicAttributeCurvedLinear> ::= 'dyn_curved_linear' '{' {<DynamicAttributeCurvedLinearProperties>} '}' \n"
	"<DynamicAttributeCurvedSpline> ::= 'dyn_curved_spline' '{' {<DynamicAttributeCurvedSplineProperties>} '}' \n"
	"<DynamicAttributeOscillate> ::= 'dyn_oscillate' '{' {<DynamicAttributeOscillateProperties>} '}' \n"
	"<DynamicAttributeRandomProperties> ::= <DynamicAttributeRandomMin> | <DynamicAttributeRandomMax> \n"
	"<DynamicAttributeCurvedLinearProperties> ::= <DynamicAttributeControlPoint> \n"
	"<DynamicAttributeCurvedSplineProperties> ::= <DynamicAttributeControlPoint> \n"
	"<DynamicAttributeOscillateProperties> ::= <DynamicAttributeOscType> | <DynamicAttributeOscFrequency> | <DynamicAttributeOscPhase> | <DynamicAttributeOscBase> | <DynamicAttributeOscAmplitude> \n"
	"<DynamicAttributeControlPoint> ::= 'control_point' <#x> <#y> \n"
	"<DynamicAttributeRandomMin> ::= 'min' <#min> \n"
	"<DynamicAttributeRandomMax> ::= 'max' <#max> \n"
	"<DynamicAttributeOscType> ::= 'oscillate_type' <Label> \n"
	"<DynamicAttributeOscFrequency> ::= 'oscillate_frequency' <#dynamicAttributeOscFrequency> \n"
	"<DynamicAttributeOscPhase> ::= 'oscillate_phase' <#dynamicAttributeOscPhase> \n"
	"<DynamicAttributeOscBase> ::= 'oscillate_base' <#dynamicAttributeOscBase> \n"
	"<DynamicAttributeOscAmplitude> ::= 'oscillate_amplitude' <#dynamicAttributeOscAmplitude> \n"

	// Particle, component rules
	"<ParticleTypeValues> ::= <VisualParticle> | <EmitterParticle> | <TechniqueParticle> | <AffectorParticle> | <SystemParticle> \n"
	"<VisualParticle> ::= 'visual_particle' \n"
	"<EmitterParticle> ::= 'emitter_particle' \n"
	"<TechniqueParticle> ::= 'technique_particle' \n"
	"<AffectorParticle> ::= 'affector_particle' \n"
	"<SystemParticle> ::= 'system_particle' \n"
	"<ComponentType> ::= <EmitterComponentType> | <TechniqueComponentType> | <AffectorComponentType> | <ObserverComponentType> \n"
	"<EmitterComponentType> ::= 'emitter_component' \n"
	"<TechniqueComponentType> ::= 'technique_component' \n"
	"<AffectorComponentType> ::= 'affector_component' \n"
	"<ObserverComponentType> ::= 'observer_component' \n"

	// Dependency
	"<StreamDependency> ::= '<<' <Dependency> \n"
	"<Dependency> ::= <CameraDependency> \n"
	"<CameraDependency> ::= 'camera_dependency' '{' {<CameraDependencyProperties>} '}' \n"
	"<CameraDependencyProperties> ::= <CameraDistanceThreshold> | <CameraIncrease> \n"
	"<CameraDistanceThreshold> ::= 'distance_threshold' <#distanceThreshold> \n"
	"<CameraIncrease> ::= 'increase' <True_False> \n"

	// Common rules
	"<Test> ::= 'test' <#test> \n"
	"<KeepLocal> ::= 'keep_local' <True_False> \n"
	"<LessGreaterThan> ::= 'less_than' | 'greater_than' \n"
	"<Position> ::= 'position' <Vector3> \n"
	"<Vector3> ::= <#x> <#y> <#z> \n"
	"<Vector4> ::= <#w> <#x> <#y> <#z> \n"
	"<Quaternion> ::= <#w> <#x> <#y> <#z> \n"
	"<Colour> ::= <#r> <#g> <#b> [<#a>]\n"
	"<Point> ::= 'point' \n"
	"<Box> ::= 'box' \n"
	"<Sphere> ::= 'sphere' \n"
	"<Plane> ::= 'plane' \n"
	"<Enabled> ::= 'enabled' <True_False> \n"
	"<Seperator> ::= -' ' \n"
	"<On_Off> ::= 'on' | 'off' \n"
	"<True_False> ::= 'true' | 'false' \n"
	"<Label> ::= <Quoted_Label> | <Unquoted_Label> \n"
	"<Flex_Label> ::= <Quoted_Label> | <Spaced_Label> \n"
	"<Quoted_Label> ::= -'\"' <Spaced_Label> -'\"' \n"
	"<Spaced_Label> ::= <Spaced_Label_Illegals> {<Spaced_Label_Illegals>} \n"
	"<Unquoted_Label> ::= <Unquoted_Label_Illegals> {<Unquoted_Label_Illegals>} \n"
	"<Spaced_Label_Illegals> ::= (!,\n\r\t{}\") \n"
	"<Unquoted_Label_Illegals> ::= (! \n\r\t{}\") \n"
	;
	// ----------------------------------- End Main Footer -----------------------------------

	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleRendererPropertiesGrammar(void)
	{
		return particleRendererPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleRendererDetailedPropertiesGrammar(void)
	{
		return particleRendererDetailedPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleEmitterPropertiesGrammar(void)
	{
		return particleEmitterPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleEmitterDetailedPropertiesGrammar(void)
	{
		return particleEmitterDetailedPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleAffectorPropertiesGrammar(void)
	{
		return particleAffectorPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleAffectorDetailedPropertiesGrammar(void)
	{
		return particleAffectorDetailedPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleObserverPropertiesGrammar(void)
	{
		return particleObserverPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleObserverDetailedPropertiesGrammar(void)
	{
		return particleObserverDetailedPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleEventHandlerPropertiesGrammar(void)
	{
		return particleEventHandlerPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleEventHandlerDetailedPropertiesGrammar(void)
	{
		return particleEventHandlerDetailedPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleBehaviourPropertiesGrammar(void)
	{
		return particleBehaviourPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getParticleBehaviourDetailedPropertiesGrammar(void)
	{
		return particleBehaviourDetailedPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getExternPropertiesGrammar(void)
	{
		return externPropertiesGrammar;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& Grammar::getExternDetailedPropertiesGrammar(void)
	{
		return externDetailedPropertiesGrammar;
	}

}
