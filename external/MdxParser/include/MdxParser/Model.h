#pragma once

#include "Attachment.h"
#include "Bone.h"
#include "Camera.h"
#include "CollisionShape.h"
#include "Geoset.h"
#include "GeosetAnimation.h"
#include "Helper.h"
#include "Light.h"
#include "Material.h"
#include "Node.h"
#include "Sequence.h"
#include "Texture.h"
#include "TextureAnimation.h"

#include <string>
#include <stdint.h>
#include <vector>
#include <array>
#include <fstream>

namespace MdxParser 
{
	typedef std::array<float, 3> pivot;

	class Model
	{
	public:
		Model();
		Model(const char* filePath);

		void write(const char* filePath);

		void addSequence(Sequence sequence);
		void addTexture(Texture texture);
		void addMaterial(Material material);
		void addGeoset(Geoset geoset);
		void addBone(Bone bone);
		void addHelper(Helper helper);
		void addPivot(pivot pivot);

		Bone* findBone(int id);
		Bone* findBone(std::string nodeName);
		Helper* findHelper(int id);
		Helper* findHelper(std::string nodeName);
		Node* findNode(int id);
		Node* findNode(std::string nodeName);

		void setVersion(uint32_t version);
		void setName(std::string name);
		void setAnimationFileName(std::string animationFileName);
		void setExtent(Extent extent);
		void setBlendTime(uint32_t blendTime);

		uint32_t getVersion() const;
		std::string getName() const;
		std::string getAnimationFileName() const;
		Extent getExtent() const;
		uint32_t getBlendTime() const;
		std::vector<Sequence>& getSequences();
		std::vector<Texture>& getTextures();
		std::vector<Material>& getMaterials();
		std::vector<TextureAnimation>& getTextureAnimations();
		std::vector<Geoset>& getGeosets();
		std::vector<GeosetAnimation>& getGeosetAnimations();
		std::vector<Bone>& getBones();
		std::vector<Helper>& getHelpers();
		std::vector<pivot>& getPivots();
		std::vector<Camera>& getCameras();
		std::vector<CollisionShape>& getCollisionShapes();
		std::vector<Node*> getNodes();

		int getSequencesCount() const;
		int getTexturesCount() const;
		int getMaterialsCount() const;
		int getTextureAnimationsCount() const;
		int getGeosetsCount() const;
		int getGeosetAnimationsCount() const;
		int getBonesCount() const;
		int getHelpersCount() const;
		int getPivotsCount() const;
		int getCamerasCount() const;
		int getCollisionShapesCount() const;
		int getNodesCount() const;

	protected:
		template <class T> void readVector(std::vector<T>& vector, std::istream& stream, uint32_t size);
		template <class T> void writeVector(std::vector<T>& vector, std::ostream& stream);

		// VERS Chunk
		uint32_t version;

		// MODL Chunk
		std::string name;
		std::string animationFileName;
		Extent extent;
		uint32_t blendTime;

		// Vector Chunks
		std::vector<Sequence> sequences; // SEQS
		std::vector<uint32_t> globalSequences; // GLBS
		std::vector<Texture> textures; // TEXS
		//std::vector<SoundTrack> soundTracks; // SNDS
		std::vector<Material> materials; // MTLS
		std::vector<TextureAnimation> textureAnimations; // TXAN
		std::vector<Geoset> geosets; // GEOS
		std::vector<GeosetAnimation> geosetAnimations; // GEOA
		std::vector<Bone> bones; // BONE
		std::vector<Light> lights; // LITE
		std::vector<Helper> helpers; // HELP
		std::vector<Attachment> attachments; // ATCH
		std::vector<pivot> pivots; // PIVT
		//std::vector<ModelEmitter> modelEmitters; // PREM
		//std::vector<QuadEmitter> quadEmitters; // PRE2
		//std::vector<RibbonEmitter> ribbonEmitters; // RIBB
		//std::vector<EventObject> eventObjects; // EVTS
		std::vector<Camera> cameras; // CAMS
		std::vector<CollisionShape> collisionShapes; // CLID

		// BPOS Chunk
		// uint32_t bindPoseCount;
		// std::vector<float> bindPoses;

		// FAFX Chunk
		// std::string faceFxTarget;
		// std::string faceFxPath;

		// CORN
		// std::vector<CornEmitter> cornEmitters;
	};
}