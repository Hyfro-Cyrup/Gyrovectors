# The Systems/ Module

 > If a module directly calls an external API or manipulates low-level engine resources, it belongs in a service module.
    If it only coordinates components and delegates to services, it belongs in Systems/.

Systems serve to bridge the gap between ECS and low-level services. 
Ideally, the core `App` should not interact with low-level services directly. 
`App` should make objects with the ECS and use a System to communicate between the object and the low-level system. 

## Example

`Rendering/` is a service module. 
It interacts with low-level OpenGL calls and is fairly naive to its inputs.
It defines the shape of data it needs fed into it (`RenderPrimitive`), but doesn't know where it comes from.

`Components/` is the ECS. 
`IRenderComponent` doesn't know about `RenderPrimitive`; it defines its shape however is best for itself.

`RenderSystem` is the bridge between these two areas.
It loops over components and converts them into primitives. 
It gets those components from the App via `RenderSystem::RenderScene`. 
