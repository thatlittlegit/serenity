set(SOURCES
    CursorTool.cpp
    Debugger/BacktraceModel.cpp
    Debugger/Debugger.cpp
    Debugger/DebugInfoWidget.cpp
    Debugger/DisassemblyModel.cpp
    Debugger/DisassemblyWidget.cpp
    Debugger/RegistersModel.cpp
    Debugger/VariablesModel.cpp
    Editor.cpp
    EditorWrapper.cpp
    FindInFilesWidget.cpp
    FormEditorWidget.cpp
    FormWidget.cpp
    Locator.cpp
    main.cpp
    ProcessStateWidget.cpp
    Project.cpp
    ProjectFile.cpp
    TerminalWrapper.cpp
    Tool.cpp
    WidgetTool.cpp
    WidgetTreeModel.cpp
    CodeDocument.cpp
)

serenity_bin(HackStudio)
target_link_libraries(HackStudio LibWeb LibMarkdown LibGUI LibGfx LibCore LibVT LibDebug LibX86)
