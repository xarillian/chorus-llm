#pragma once

#include "chorus/core/inference_engine.hpp"

#include <memory>
#include <string>
#include <variant>

namespace Chorus {

enum class Provider { Llama, Echo };

std::unique_ptr<InferenceEngine> make_engine(Provider provider);

/*
 * Shapes a host-selected model according to the provider's asset policy.
 *
 * Hosts must check for a rejection before calling `Chorus::ChorusRuntime::load_engine`,
 * because an accepted load retires the current engine even when the new one later fails.
 *
 * Returns:
 *  - `Chorus::InitialModelSpec`: the model the provider boots with.
 *  - `Chorus::RequestRejection`: the provider requires a model source and none was given.
 */
std::variant<InitialModelSpec, RequestRejection>
make_initial_model_spec(Provider provider, std::string model_id, std::string model_source);

/// Returns what a provider supports and how it may be configured before initialization.
EngineCapabilities describe_provider_capabilities(Provider provider);

} // namespace Chorus
