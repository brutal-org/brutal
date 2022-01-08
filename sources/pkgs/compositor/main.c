static void on_attach(BalComponent *self, Capability capability)
{
}

static void on_detach(BalComponent *self, BalCapability capability)
{
    /* ... */
}

WmServerVtable wm_server_vtable = {
    .create_window = /* ... */,
    .destroy_window = /* ... */,
};

InputSinkVtable input_sink_vtable = {
    .on_mouse_event = /* ... */,
    .on_keyboard_event = /* ... */,
};

int bal_main(BalComponent *self)
{
    self->on_attach = on_attach;
    self->on_detach = on_detach;

    BalCapability wm_server_capability = wm_server_provide(self, &wm_server_vtable);
    BalCapability input_sink_capability = input_sink_provide(self, &input_sink_vtable);

    Vec(BalCapabilities) inputs = bal_component_query((BalComponentQuery) {
        .type = BAL_CAPABILITY_COMPONENT,
        .component = {
            .proto = INPUT_SOURCE_PROTO_ID,
        };
    });

    vec_foreach_v(cap, &inputs)
    {
        input_source_register_sink(self, cap.component, wm_server_capability);
    }

    vec_deinit(&inputs);

    return bal_component_run(self);
}