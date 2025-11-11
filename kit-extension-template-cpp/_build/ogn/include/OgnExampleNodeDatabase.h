#pragma once

#include <omni/graph/core/ogn/UsdTypes.h>
using namespace pxr;

#include <carb/InterfaceUtils.h>
#include <omni/graph/core/NodeTypeRegistrar.h>
#include <omni/graph/core/iComputeGraph.h>
#include <omni/graph/core/CppWrappers.h>
#include <omni/fabric/Enums.h>
using omni::fabric::PtrToPtrKind;
#include <map>
#include <vector>
#include <tuple>
#include <omni/graph/core/OgnHelpers.h>
#include <array>
#include <omni/graph/core/Type.h>
#include <omni/graph/core/ogn/ArrayAttribute.h>
#include <omni/graph/core/ogn/Bundle.h>
#include <omni/graph/core/ogn/SimpleAttribute.h>
#include <omni/graph/core/ogn/SimpleRuntimeAttribute.h>
#include <omni/graph/core/tuple.h>

namespace OgnExampleNodeAttributes
{
namespace inputs
{
using disable_t = const bool&;
ogn::AttributeInitializer<const bool, ogn::kOgnInput> disable("inputs:disable", "bool", kExtendedAttributeType_Regular, true);
using geometry_t = const ConstBundleHandle&;
ogn::AttributeInitializer<const ConstBundleHandle, ogn::kOgnInput> geometry("inputs:geometry", "bundle", kExtendedAttributeType_Regular);
using offset_t = const ogn::RuntimeAttribute<ogn::kOgnInput, ogn::kCpu>&;
ogn::AttributeInitializer<const ogn::RuntimeAttribute<ogn::kOgnInput, ogn::kCpu>, ogn::kOgnInput> offset("inputs:offset", "float,float[3]", kExtendedAttributeType_Union);
}
namespace outputs
{
using points_t = ogn::array<pxr::GfVec3f>&;
ogn::AttributeInitializer<pxr::GfVec3f*, ogn::kOgnOutput> points("outputs:points", "float3[]", kExtendedAttributeType_Regular, nullptr, 0);
}
namespace state
{
}
}
using namespace OgnExampleNodeAttributes;
namespace IOgnExampleNode
{
// This is an example OmniGraph node. It adds a fixed offset specified by the 'offset'
// input attribute to any attributes within the input bundle 'geometry' that have type
// 'float[3]' and appends these points to the output 'points' array. The offset is ignored
// if the input 'disable' is set to True. This gives a rough guideline of how to access
// some of the most common data types. See the online OmniGraph documentation for more
// detail on what you are able to do within a node.
class OgnExampleNodeDatabase : public omni::graph::core::ogn::OmniGraphDatabase
{
public:
    template <typename StateInformation>
    CARB_DEPRECATED("sInternalState is deprecated. Use sSharedState or sPerInstanceState instead")
    static StateInformation& sInternalState(const NodeObj& nodeObj, InstanceIndex index = {kAuthoringGraphIndex}) {
        return sm_stateManagerOgnExampleNode.getState<StateInformation>(nodeObj.nodeHandle, index);
    }
    template <typename StateInformation>
    static StateInformation& sSharedState(const NodeObj& nodeObj) {
        return sm_stateManagerOgnExampleNode.getState<StateInformation>(nodeObj.nodeHandle, kAuthoringGraphIndex);
    }
    template <typename StateInformation>
    static StateInformation& sPerInstanceState(const NodeObj& nodeObj, InstanceIndex index) {
        return sm_stateManagerOgnExampleNode.getState<StateInformation>(nodeObj.nodeHandle, index);
    }
    template <typename StateInformation>
    static StateInformation& sPerInstanceState(const NodeObj& nodeObj, GraphInstanceID instanceId) {
        return sm_stateManagerOgnExampleNode.getState<StateInformation>(nodeObj.nodeHandle, instanceId);
    }
    template <typename StateInformation>
    CARB_DEPRECATED("internalState is deprecated. Use sharedState or perInstanceState instead")
    StateInformation& internalState(size_t relativeIdx = 0) {
        return sInternalState<StateInformation>(abi_node(), m_offset + relativeIdx);
    }
    template <typename StateInformation>
    StateInformation& sharedState() {
        return sSharedState<StateInformation>(abi_node());
    }
    template <typename StateInformation>
    StateInformation& perInstanceState(size_t relativeIdx = 0) {
        return sPerInstanceState<StateInformation>(abi_node(), m_offset + relativeIdx);
    }
    template <typename StateInformation>
    StateInformation& perInstanceState(GraphInstanceID instanceId) {
        return sPerInstanceState<StateInformation>(abi_node(), instanceId);
    }
    static ogn::StateManager sm_stateManagerOgnExampleNode;
    static std::tuple<int, int, int>sm_generatorVersionOgnExampleNode;
    static std::tuple<int, int, int>sm_targetVersionOgnExampleNode;
    static constexpr size_t staticAttributeCount = 6;
    std::vector<ogn::DynamicInput> m_dynamicInputs;
    std::vector<ogn::DynamicOutput> m_dynamicOutputs;
    std::vector<ogn::DynamicState> m_dynamicStates;
    std::vector<NameToken> m_mappedAttributes;
    bool m_canCachePointers{true};

    struct inputsT {
        inputsT(size_t const& offset)
        : disable{offset}
        , geometry{offset}
        , offset{offset}
        {}
        ogn::SimpleInput<const bool,ogn::kCpu> disable;
        ogn::BundleAttribute<ogn::kOgnInput,ogn::kCpu> geometry;
        ogn::SimpleInput<const ogn::RuntimeAttribute<ogn::kOgnInput, ogn::kCpu>,ogn::kCpu> offset;
    } inputs;

    struct outputsT {
        outputsT(size_t const& offset)
        : points{offset}
        {}
        ogn::ArrayOutput<pxr::GfVec3f,ogn::kCpu> points;
    } outputs;

    //Only use this constructor for temporary stack-allocated object:
    OgnExampleNodeDatabase(NodeObj const& nodeObjParam)
    : OmniGraphDatabase()
    , inputs{m_offset.index}
    , outputs{m_offset.index}
    {
        GraphContextObj const* contexts = nullptr;
        NodeObj const* nodes = nullptr;
        size_t handleCount = nodeObjParam.iNode->getAutoInstances(nodeObjParam, contexts, nodes);
        _ctor(contexts, nodes, handleCount);
        _init();
    }

    CARB_DEPRECATED("Passing the graph context to the temporary stack allocated database is not necessary anymore: you can safely remove this parameter")
    OgnExampleNodeDatabase(GraphContextObj const&, NodeObj const& nodeObjParam)
    : OgnExampleNodeDatabase(nodeObjParam)
    {}

    //Main constructor
    OgnExampleNodeDatabase(GraphContextObj const* contextObjParam, NodeObj const* nodeObjParam, size_t handleCount)
    : OmniGraphDatabase()
    , inputs{m_offset.index}
    , outputs{m_offset.index}
    {
        _ctor(contextObjParam, nodeObjParam, handleCount);
        _init();
    }

private:
    void _init() {
        GraphContextObj const& contextObj = abi_context();
        NodeObj const& nodeObj = abi_node();
        {
            auto inputDataHandles0 = getAttributesR<
                ConstAttributeDataHandle
                >(contextObj, nodeObj.nodeContextHandle, std::make_tuple(
                    inputs::disable.m_token
                )
            , kAccordingToContextIndex);
            auto inputDataBundleHandles0 = getAttributesR<
                ConstAttributeDataHandle
                >(contextObj, nodeObj.nodeContextHandle, std::make_tuple(
                    inputs::geometry.m_token
                )
            , kAccordingToContextIndex);
            auto outputDataHandles0 = getAttributesW<
                AttributeDataHandle
                >(contextObj, nodeObj.nodeContextHandle, std::make_tuple(
                    outputs::points.m_token
                )
            , kAccordingToContextIndex);
            inputs.disable.setContext(contextObj);
            inputs.disable.setHandle(std::get<0>(inputDataHandles0));
            inputs.geometry.setContext(contextObj);
            inputs.geometry.setHandle(std::get<0>(inputDataBundleHandles0));
            outputs.points.setContext(contextObj);
            outputs.points.setHandle(std::get<0>(outputDataHandles0));
        }
        {
            ConstAttributeDataHandle __h;
            AttributeObj __a;
            __a = nodeObj.iNode->getAttributeByToken(nodeObj, inputs::offset.m_token);
            __h = __a.iAttribute->getConstAttributeDataHandle(__a, kAccordingToContextIndex);
            const_cast<typename std::remove_const_t<ogn::RuntimeAttribute<ogn::kOgnInput, ogn::kCpu>&>>(inputs.offset()).reset(contextObj, __h, __a);

        }
        tryGetDynamicAttributes<AttributePortType::kAttributePortType_Input>(staticAttributeCount, m_dynamicInputs);
        tryGetDynamicAttributes<AttributePortType::kAttributePortType_Output>(staticAttributeCount, m_dynamicOutputs);
        tryGetDynamicAttributes<AttributePortType::kAttributePortType_State>(staticAttributeCount, m_dynamicStates);

        collectMappedAttributes(m_mappedAttributes);

        m_canCachePointers = contextObj.iContext->canCacheAttributePointers ?
                                 contextObj.iContext->canCacheAttributePointers(contextObj) : true;
    }

public:
    static void initializeType(const NodeTypeObj& nodeTypeObj)
    {
        const INodeType* iNodeType = nodeTypeObj.iNodeType;
        auto iTokenPtr = carb::getCachedInterface<omni::fabric::IToken>();
        if( ! iTokenPtr ) {
            CARB_LOG_ERROR("IToken not found when initializing omni.example.cpp.omnigraph_node.ExampleNode");
            return;
        }
        auto& iToken{ *iTokenPtr };

        inputs::disable.initialize(iToken, *iNodeType, nodeTypeObj);
        inputs::geometry.initialize(iToken, *iNodeType, nodeTypeObj);
        inputs::offset.initialize(iToken, *iNodeType, nodeTypeObj);

        outputs::points.initialize(iToken, *iNodeType, nodeTypeObj);

        iNodeType->setMetadata(nodeTypeObj, kOgnMetadataExtension, "omni.example.cpp.omnigraph_node");
        iNodeType->setMetadata(nodeTypeObj, kOgnMetadataUiName, "Example Offset Node");
        iNodeType->setMetadata(nodeTypeObj, kOgnMetadataCategories, "tutorials");
        iNodeType->setMetadata(nodeTypeObj, kOgnMetadataDescription, "This is an example OmniGraph node. It adds a fixed offset specified by the 'offset' input attribute to any attributes within the input bundle 'geometry' that have type 'float[3]' and appends these points to the output 'points' array. The offset is ignored if the input 'disable' is set to True. This gives a rough guideline of how to access some of the most common data types. See the online OmniGraph documentation for more detail on what you are able to do within a node.");
    }
    static void initialize(const GraphContextObj&, const NodeObj& nodeObj)
    {
        const INode* iNode = nodeObj.iNode;
        AttributeObj attr;
        attr = iNode->getAttributeByToken(nodeObj, inputs::disable.token());
        attr.iAttribute->setMetadata(attr, kOgnMetadataDescription, "Set this to false to prevent any offset from being applied to points");
        attr.iAttribute->setMetadata(attr, kOgnMetadataUiName, "Disable Offsets");
        attr.iAttribute->setMetadata(attr, kOgnMetadataDefault, "true");
        attr = iNode->getAttributeByToken(nodeObj, inputs::geometry.token());
        attr.iAttribute->setMetadata(attr, kOgnMetadataDescription, "A bundle is a collection of attributes with their own names. This node inspects the bundle\nto find attributes of type 'float[3]'. If found then it extracts each member, adds the\noffset to them, and appends the results to the output 'points' array attribute.");
        attr.iAttribute->setMetadata(attr, kOgnMetadataUiName, "Geometry To Offset");
        attr = iNode->getAttributeByToken(nodeObj, inputs::offset.token());
        attr.iAttribute->setMetadata(attr, kOgnMetadataDescription, "Amount of offset to be applied to all of the values in the 'points' bundled attribute.\nIf the attribute value is resolved to be a float[3] then that value is applied evenly\nto every point. If the attribute value is resolved to be a plain float then that value\nis used as a common offset for each of the x, y, and z components of the points.");
        attr.iAttribute->setMetadata(attr, kOgnMetadataUiName, "Offset Amount");
        attr = iNode->getAttributeByToken(nodeObj, outputs::points.token());
        attr.iAttribute->setMetadata(attr, kOgnMetadataDescription, "Set of points extracted from the input 'geometry' bundle with the offsets added. If there is\nno matching attribute in the geometry bundle then this will be an empty array.");
        attr.iAttribute->setMetadata(attr, kOgnMetadataUiName, "Offset Points");
    }
    std::vector<ogn::DynamicInput> const& getDynamicInputs() const
    {
        return m_dynamicInputs;
    }
    gsl::span<ogn::DynamicOutput> getDynamicOutputs()
    {
        return m_dynamicOutputs;
    }
    gsl::span<ogn::DynamicState> getDynamicStates()
    {
        return m_dynamicStates;
    }
    static void release(const NodeObj& nodeObj, GraphInstanceID instanceID)
    {
        sm_stateManagerOgnExampleNode.removeState(nodeObj.nodeHandle, instanceID);
    }
    bool validate() const {
        return validateNode()
            && inputs.disable.isValid()
            && inputs.geometry.isValid()
            && inputs.offset().resolved()
            && outputs.points.isValid()
        ;
    }
    void preCompute() {
        if(m_canCachePointers == false) {
            inputs.disable.invalidateCachedPointer();
            inputs.geometry.invalidateCachedPointer();
            inputs.offset.invalidateCachedPointer();
            outputs.points.invalidateCachedPointer();
            return;
        }
        outputs.points.invalidateCachedPointer();
        for(NameToken const& attrib : m_mappedAttributes) {
            if(attrib == inputs::disable.m_token) {
                inputs.disable.invalidateCachedPointer();
                continue;
            }
            if(attrib == inputs::offset.m_token) {
                inputs.offset.invalidateCachedPointer();
                continue;
            }
            if(attrib == outputs::points.m_token) {
                outputs.points.invalidateCachedPointer();
                continue;
            }
            bool found = false;
            for (auto& __a : m_dynamicInputs) {
                if (__a().name() == attrib) {
                    __a.invalidateCachedPointer();
                    found = true;
                    break;
                }
            }
            if(found) continue;
            for (auto& __a : m_dynamicOutputs) {
                if (__a().name() == attrib) {
                    __a.invalidateCachedPointer();
                    found = true;
                    break;
                }
            }
            if(found) continue;
            for (auto& __a : m_dynamicStates) {
                if (__a().name() == attrib) {
                    __a.invalidateCachedPointer();
                    found = true;
                    break;
                }
            }
            if(found) continue;
        }
    }
    bool canVectorize() const {
        if( !inputs.disable.canVectorize()
            || !inputs.offset.canVectorize()
        ) return false;
        for (auto const& __a : m_dynamicInputs) {
            if(!__a.canVectorize()) return false;
        }
        for (auto const& __a : m_dynamicOutputs) {
            if(!__a.canVectorize()) return false;
        }
        for (auto const& __a : m_dynamicStates) {
            if(!__a.canVectorize()) return false;
        }
        return true;
    }
    void onTypeResolutionChanged(AttributeObj const& attr) {
        if (! attr.isValid()) return;
        NameToken token = attr.iAttribute->getNameToken(attr);
        if(inputs::offset.m_token == token) {
            inputs.offset.fetchDetails(attr);
            return;
        }
        for (auto& __a : m_dynamicInputs) {
            if (__a().name() == token) {
                __a.fetchDetails(attr);
                return;
            }
        }
        for (auto& __a : m_dynamicOutputs) {
            if (__a().name() == token) {
                __a.fetchDetails(attr);
                return;
            }
        }
        for (auto& __a : m_dynamicStates) {
            if (__a().name() == token) {
                __a.fetchDetails(attr);
                return;
            }
        }
    }
    void onDynamicAttributesChanged(AttributeObj const& attribute, bool isAttributeCreated) {
        onDynamicAttributeCreatedOrRemoved(m_dynamicInputs, m_dynamicOutputs, m_dynamicStates, attribute, isAttributeCreated);
    }
    void onDataLocationChanged(AttributeObj const& attr) {
        if (! attr.isValid()) return;
        updateMappedAttributes(m_mappedAttributes, attr);
        NameToken token = attr.iAttribute->getNameToken(attr);
        if(token == inputs::disable.m_token) {
            ConstAttributeDataHandle hdl = attr.iAttribute->getConstAttributeDataHandle(attr, m_offset);
            inputs.disable.setHandle(hdl);
            return;
        }
        if(token == inputs::geometry.m_token) {
            ConstAttributeDataHandle hdl = attr.iAttribute->getConstAttributeDataHandle(attr, m_offset);
            inputs.geometry.setHandle(hdl);
            return;
        }
        if(token == inputs::offset.m_token) {
            inputs.offset.fetchDetails(attr);
            return;
        }
        if(token == outputs::points.m_token) {
            AttributeDataHandle hdl = attr.iAttribute->getAttributeDataHandle(attr, m_offset);
            outputs.points.setHandle(hdl);
            return;
        }
        for (auto& __a : m_dynamicInputs) {
            if (__a().name() == token) {
                __a.fetchDetails(attr);
                return;
            }
        }
        for (auto& __a : m_dynamicOutputs) {
            if (__a().name() == token) {
                __a.fetchDetails(attr);
                return;
            }
        }
        for (auto& __a : m_dynamicStates) {
            if (__a().name() == token) {
                __a.fetchDetails(attr);
                return;
            }
        }
    }
};
ogn::StateManager OgnExampleNodeDatabase::sm_stateManagerOgnExampleNode;
std::tuple<int, int, int> OgnExampleNodeDatabase::sm_generatorVersionOgnExampleNode{std::make_tuple(1,79,3)};
std::tuple<int, int, int> OgnExampleNodeDatabase::sm_targetVersionOgnExampleNode{std::make_tuple(2,184,7)};
}
using namespace IOgnExampleNode;
#define REGISTER_OGN_NODE() \
namespace { \
    ogn::NodeTypeBootstrapImpl<OgnExampleNode, OgnExampleNodeDatabase> s_registration("omni.example.cpp.omnigraph_node.ExampleNode", 1, "omni.example.cpp.omnigraph_node"); \
}
