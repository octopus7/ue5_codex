import unreal


def describe_widget(widget, depth=0):
    indent = "  " * depth
    name = widget.get_name()
    cls = widget.get_class().get_name()
    print(f"{indent}{name} [{cls}]")

    try:
        visibility = widget.get_editor_property("visibility")
        print(f"{indent}  visibility={visibility}")
    except Exception:
        pass

    if isinstance(widget, unreal.Image):
        try:
            brush = widget.get_editor_property("brush")
            resource = brush.get_editor_property("resource_object")
            image_size = brush.get_editor_property("image_size")
            color = widget.get_editor_property("color_and_opacity")
            print(
                f"{indent}  brush_resource={resource.get_path_name() if resource else None} "
                f"image_size={image_size} color={color}"
            )
        except Exception as exc:
            print(f"{indent}  image_props_error={exc}")

    if isinstance(widget, unreal.Border):
        try:
            padding = widget.get_editor_property("padding")
            brush_color = widget.get_editor_property("brush_color")
            print(f"{indent}  padding={padding} brush_color={brush_color}")
        except Exception as exc:
            print(f"{indent}  border_props_error={exc}")

    if isinstance(widget, unreal.TextBlock):
        try:
            text = widget.get_editor_property("text")
            color = widget.get_editor_property("color_and_opacity")
            print(f"{indent}  text={text} color={color}")
        except Exception as exc:
            print(f"{indent}  text_props_error={exc}")

    slot = widget.slot
    if slot:
        slot_cls = slot.get_class().get_name()
        print(f"{indent}  slot={slot_cls}")
        if isinstance(slot, unreal.CanvasPanelSlot):
            try:
                layout = slot.get_editor_property("layout_data")
                offsets = layout.get_editor_property("offsets")
                anchors = layout.get_editor_property("anchors")
                alignment = slot.get_editor_property("alignment")
                auto_size = slot.get_editor_property("auto_size")
                z_order = slot.get_editor_property("z_order")
                print(
                    f"{indent}  canvas offsets={offsets} anchors={anchors} "
                    f"alignment={alignment} auto_size={auto_size} z_order={z_order}"
                )
            except Exception as exc:
                print(f"{indent}  canvas_props_error={exc}")
        elif isinstance(slot, unreal.OverlaySlot):
            try:
                h_align = slot.get_editor_property("horizontal_alignment")
                v_align = slot.get_editor_property("vertical_alignment")
                print(f"{indent}  overlay h={h_align} v={v_align}")
            except Exception as exc:
                print(f"{indent}  overlay_props_error={exc}")

    if isinstance(widget, unreal.PanelWidget):
        for child in widget.get_children():
            describe_widget(child, depth + 1)


asset = unreal.EditorAssetLibrary.load_asset("/Game/UI/Interaction/WBP_InteractionIndicator")
if not asset:
    raise RuntimeError("Failed to load WBP_InteractionIndicator")

print(f"asset={asset.get_path_name()} class={asset.get_class().get_name()}")
widget_tree = asset.get_editor_property("widget_tree")
if not widget_tree:
    raise RuntimeError("Widget blueprint has no widget_tree")

root = widget_tree.get_editor_property("root_widget")
if not root:
    raise RuntimeError("Widget tree has no root widget")

describe_widget(root)
