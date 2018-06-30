SET( ${PROJECT_NAME}_HEADER_FILES
	src/presentationenginemanager.h
)

SET( ${PROJECT_NAME}_SOURCE_FILES
	${${PROJECT_NAME}_HEADER_FILES}
	src/presentationenginemanager.cpp
	src/FormatterScheduler.cpp
	src/animation/animation.cpp
	src/animation/transitionanimation.cpp
	src/adapters/ApplicationPlayerAdapter.cpp
	src/adapters/PlayerAdapterManager.cpp
	src/adapters/FormatterPlayerAdapter.cpp
	src/adapters/RuleAdapter.cpp
	src/model/event/eventmanager.cpp
	src/model/event/FormatterEvent.cpp
	src/model/event/transition/EventTransition.cpp
	src/model/event/transition/EndEventTransition.cpp
	src/model/event/transition/BeginEventTransition.cpp
	src/model/event/transition/EventTransitionManager.cpp
	src/model/event/AnchorEvent.cpp
	src/model/event/PresentationEvent.cpp
	src/model/event/SelectionEvent.cpp
	src/model/event/AttributionEvent.cpp
	src/model/presentation/CascadingDescriptor.cpp
	src/model/presentation/FormatterRegion.cpp
	src/model/switches/ExecutionObjectSwitch.cpp
	src/model/switches/SwitchEvent.cpp
	src/model/components/ExecutionObject.cpp
	src/model/components/ApplicationExecutionObject.cpp
	src/model/components/CompositeExecutionObject.cpp
	src/model/components/NodeNesting.cpp
	src/model/link/LinkValueAssessment.cpp
	src/model/link/FormatterLink.cpp
	src/model/link/LinkCompoundTriggerCondition.cpp
	src/model/link/LinkTransitionTriggerCondition.cpp
	src/model/link/LinkAttributeAssessment.cpp
	src/model/link/LinkRepeatAction.cpp
	src/model/link/LinkSimpleAction.cpp
	src/model/link/FormatterCausalLink.cpp
	src/model/link/LinkTriggerCondition.cpp
	src/model/link/LinkAndCompoundTriggerCondition.cpp
	src/model/link/LinkAssessmentStatement.cpp
	src/model/link/LinkAssignmentAction.cpp
	src/model/link/LinkCompoundStatement.cpp
	src/model/link/LinkAction.cpp
	src/model/link/LinkCompoundAction.cpp
	src/FormatterMediator.cpp
	src/emconverter/FormatterConverter.cpp
	src/emconverter/FormatterLinkConverter.cpp
	src/privatebase/PrivateBaseManager.cpp
	src/focus/FormatterFocusManager.cpp
)