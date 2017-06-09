#include "wx/wx.h"
#include "wx/aui/aui.h"
#include "wx/treectrl.h"
#include "tod/node.h"
#include "tod/kernel.h"
#include "tod/serializer.h"
#include "tod/type.h"
#include "tod/graphics/renderer.h"

typedef std::list<tod::ObjRef<tod::Node>> Selections;

class TodEditorStyle
{
public:
    static const wxColour backgroundColor;
    static const wxColour editboxBackgroundColor;
    static const wxColour normalFontColor;
    static const wxColour disableFontColor;
    static const wxColour captionBackgroundColor;
    static const wxColour captionFontColor;
    static const wxFont normalFont;
    static const wxFont captionFont;
};

const wxColour TodEditorStyle::backgroundColor(40, 43, 54);
const wxColour TodEditorStyle::editboxBackgroundColor(33, 35, 44);
const wxColour TodEditorStyle::normalFontColor(251, 251, 251);
const wxColour TodEditorStyle::disableFontColor(128, 128, 128);
const wxColour TodEditorStyle::captionBackgroundColor(54, 58, 73);
const wxColour TodEditorStyle::captionFontColor(251, 251, 251);
const wxFont TodEditorStyle::normalFont(wxFontInfo(10).FaceName("Consolas"));
const wxFont TodEditorStyle::captionFont(wxFontInfo(10).FaceName("Consolas").Bold());




class Inspector : public wxWindow
{
public:
    Inspector(wxWindow* parent):wxWindow(parent, -1)
    {
        this->SetBackgroundColour(TodEditorStyle::backgroundColor);
        this->SetForegroundColour(TodEditorStyle::normalFontColor);
        
        auto sizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(sizer);
        
        
        
        auto node_group = new wxWindow(this, -1);
        
        //--------------------------
        // DerivedNode
        //--------------------------
        // + Component
        // + Component
        //--------------------------
        //--------------------------
        // BaseNode
        //--------------------------
        // + Component
        //
        //--------------------------
    }
    
    void setSelections(Selections& selections)
    {
        this->selections = selections;
        this->generate_property_grid();
    }
    
private:
    class FolderPanel : public wxWindow
    {
    public:
        FolderPanel(wxWindow* parent, const wxString& subject):
        wxWindow(parent, -1),
        subjectPanel(nullptr),
        container(nullptr)
        {
            wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            this->SetSizer(sizer);
            this->SetBackgroundColour(TodEditorStyle::captionBackgroundColor);
            
            //subject
            this->subjectPanel = new SubjectPanel(this, subject);
            sizer->Add(this->subjectPanel, 0, wxEXPAND);
        }
        void setContainer(wxWindow* container)
        {
            this->container = container;
            this->GetSizer()->Clear();
            this->GetSizer()->Add(this->subjectPanel, 0, wxEXPAND);
            this->GetSizer()->AddSpacer(1);
            this->GetSizer()->Add(this->container, 0, wxEXPAND);
        }
        
        void fold()
        {
            this->container->Hide();
            this->GetGrandParent()->Layout();
        }
        
        void unfold()
        {
            this->container->Show();
            this->GetGrandParent()->Layout();
        }
        
    private:
        class SubjectPanel : public wxWindow
        {
        public:
            SubjectPanel(wxWindow* parent, const wxString& subject):
            wxWindow(parent, -1),
            subject(subject)
            {
                this->SetFont(TodEditorStyle::captionFont);
                this->SetForegroundColour(TodEditorStyle::captionFontColor);
                this->SetBackgroundColour(TodEditorStyle::captionBackgroundColor);
                auto size = this->GetTextExtent(this->subject);
                this->SetMinSize(wxSize(size.GetWidth(), size.GetHeight() + 8));
                this->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event)
                {
                    FolderPanel* folder_panel = static_cast<FolderPanel*>(this->GetParent());
                    if (folder_panel->container->IsShown()) folder_panel->fold();
                    else folder_panel->unfold();
                });
                new wxStaticText(this, -1, subject, wxPoint(10, 3));
            }
            wxString subject;
        };
        
    private:
        SubjectPanel* subjectPanel;
        wxWindow* container;
    };
    
    class PropertyItem : public wxWindow
    {
    public:
        PropertyItem(wxWindow* parent):
        wxWindow(parent, -1)
        {
            wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
            this->SetSizer(sizer);
            this->SetBackgroundColour(TodEditorStyle::backgroundColor);
        }
        void setTarget(tod::Node* node, tod::Property* prop)
        {
            if (this->node == node && this->property == prop) return;
            this->node = node;
            this->property = prop;
            this->GetSizer()->Clear(true);
            
            //property name
            wxStaticText* prop_name_static_text = new wxStaticText(this, -1, prop->getName());
            prop_name_static_text->SetFont(TodEditorStyle::normalFont);
            if (this->property->isReadOnly())
                prop_name_static_text->SetForegroundColour(TodEditorStyle::disableFontColor);
            else prop_name_static_text->SetForegroundColour(TodEditorStyle::normalFontColor);
            auto prop_name_size = this->GetTextExtent(prop->getName());
            prop_name_static_text->SetMinSize(wxSize(100, prop_name_size.GetHeight()));
            prop_name_static_text->SetMaxSize(wxSize(100, prop_name_size.GetHeight()));
            this->GetSizer()->AddSpacer(10);
            this->GetSizer()->Add(prop_name_static_text, 1, wxALIGN_CENTRE_VERTICAL);
            this->GetSizer()->AddSpacer(1);
            
            //property editor
            wxWindow* editor = this->create_editor(node, prop);
            editor->SetFont(TodEditorStyle::normalFont);
            if (this->property->isReadOnly()) editor->SetForegroundColour(TodEditorStyle::disableFontColor);
            else editor->SetForegroundColour(TodEditorStyle::normalFontColor);
            this->GetSizer()->Add(editor, 1, wxALIGN_CENTRE_VERTICAL);
        }
        
    public:
        wxWindow* create_editor(tod::Node* node, tod::Property* prop)
        {
            if (prop->isEqualType(typeid(bool)))
            {
                typedef tod::SimpleProperty<bool> SP;
                SP* sprop = static_cast<SP*>(prop);
                
                wxCheckBox* checkbox = new wxCheckBox(this, -1, wxEmptyString);
                checkbox->SetValue(sprop->get(node));
                if (prop->isReadOnly()) checkbox->Disable();
                checkbox->Bind(wxEVT_CHECKBOX, [this, sprop, checkbox](wxCommandEvent& event)
                {
                    sprop->set(this->node, checkbox->IsChecked());
                });
                
                return checkbox;
            }
            else
            {
                tod::String value;
                prop->toString(node, value);
                
                int option = wxBORDER_NONE | wxTE_PROCESS_ENTER;
                if (prop->isReadOnly()) option |= wxTE_READONLY;
                wxTextCtrl* text_ctrl = new wxTextCtrl(this, -1, value, wxDefaultPosition, wxDefaultSize, option);
                text_ctrl->SetMinSize(wxSize(100, 16));
                text_ctrl->SetMaxSize(wxSize(9999, 16));
                text_ctrl->SetBackgroundColour(TodEditorStyle::editboxBackgroundColor);
                text_ctrl->Bind(wxEVT_TEXT_ENTER, [this, text_ctrl](wxCommandEvent& event)
                {
                    this->property->fromString(this->node, text_ctrl->GetValue().ToStdString());
                });
                return text_ctrl;
            }
            
            /*if (prop->isEnumable())
            {
                if (TypeId<int>::check(prop->getType()))
                {
                    typedef EnumProperty<int> EP;
                    EP* eprop = static_cast<EP*>(prop);
                    wxArrayString as;
                    for (auto i : eprop->getEnumList(node)) as.Add(i);
                    
                    int option = wxBORDER_SIMPLE | wxTE_PROCESS_ENTER;
                    if (prop->isReadOnly()) option |= wxTE_READONLY;
                    wxComboBox* combo = new wxComboBox(this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, as, option);
                    
                    return combo;
                }
            }
            else if (TypeId<bool>::check(prop->getType()))
            {
                typedef SimpleProperty<bool> SP;
                SP* sprop = static_cast<SP*>(prop);
                
                wxCheckBox* checkbox = new wxCheckBox(this, -1, wxEmptyString);
                checkbox->SetValue(sprop->get(node));
                if (prop->isReadOnly()) checkbox->Disable();
                
                return checkbox;
            }
            else if (
                     TypeId<int>::check(prop->getType()) ||
                     TypeId<unsigned int>::check(prop->getType()) ||
                     TypeId<int32>::check(prop->getType()) ||
                     TypeId<uint32>::check(prop->getType()) ||
                     TypeId<float>::check(prop->getType()) ||
                     TypeId<double>::check(prop->getType()) ||
                     TypeId<const char*>::check(prop->getType()) ||
                     TypeId<const String&>::check(prop->getType()))
            {
                String value;
                prop->toString(node, value);
                
                int option = wxBORDER_NONE | wxTE_PROCESS_ENTER;
                if (prop->isReadOnly()) option |= wxTE_READONLY;
                wxTextCtrl* text_ctrl = new wxTextCtrl(this, -1, value, wxDefaultPosition, wxDefaultSize, option);
                text_ctrl->SetMinSize(wxSize(100, 16));
                text_ctrl->SetMaxSize(wxSize(9999, 16));
                return text_ctrl;
            }
            else if (TypeId<const Color&>::check(prop->getType()))
            {
                wxColourPickerCtrl* colour_picker_ctrl = new wxColourPickerCtrl(this, -1, wxStockGDI::COLOUR_BLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_USE_TEXTCTRL);
                //colour_picker_ctrl->Connect(wxEVT_COLOURPICKER_CHANGED, (wxObjectEventFunction)&PropertyEditSlot::on_update_colour_picker, 0, this);
                if (prop->isReadOnly()) colour_picker_ctrl->Disable();
                
                wxTextCtrl* text_ctrl = colour_picker_ctrl->GetTextCtrl();
                text_ctrl->SetWindowStyleFlag(wxBORDER_NONE);
                text_ctrl->SetMinSize(wxSize(100, 16));
                text_ctrl->SetMaxSize(wxSize(9999, 16));
                if (this->property->isReadOnly()) text_ctrl->SetForegroundColour(TodEditorInspector::s_textColor2);
                else text_ctrl->SetForegroundColour(TodEditorInspector::s_textColor1);
                text_ctrl->SetBackgroundColour(TodEditorInspector::s_backgroundColor1);
                return colour_picker_ctrl;
            }*/
            
            return nullptr;
        }
        
    public:
        tod::Node* node;
        tod::Property* property;
    };
    
    class PropertyListPanel : public wxWindow
    {
    public:
        PropertyListPanel(wxWindow* parent, tod::Type* type, tod::Node* node):
        wxWindow(parent, -1)
        {
            this->SetBackgroundColour(TodEditorStyle::backgroundColor);
            this->SetSizer(new wxBoxSizer(wxVERTICAL));
            
            this->setup(type, node);
        }
        
        void setup(tod::Type* type, tod::Node* node)
        {
            FolderPanel* folder_panel = new FolderPanel(this, type->getName());
            this->GetSizer()->Add(folder_panel, 0, wxEXPAND);
            
            wxWindow* property_panel = new wxWindow(folder_panel, -1);
            property_panel->SetBackgroundColour(TodEditorStyle::backgroundColor);
            folder_panel->setContainer(property_panel);
            wxBoxSizer* property_panel_sizer = new wxBoxSizer(wxVERTICAL);
            property_panel->SetSizer(property_panel_sizer);
            
            for (auto& p : type->getPropertyOrder())
            {
                PropertyItem* pitem = new PropertyItem(property_panel);
                pitem->setTarget(node, p);
                property_panel_sizer->Add(pitem, 0, wxEXPAND);
                property_panel_sizer->AddSpacer(1);
            }
        }
    };
    
private:
    void generate_property_grid()
    {
        auto gct = [](const Selections& nodes) -> tod::Type*
        {
            typedef std::stack<tod::Type*> InheritStruct;
            typedef std::list<InheritStruct> InheritStructList;
            InheritStructList l;
            for (auto i : nodes)
            {
                l.push_back(InheritStruct());
                InheritStruct& is = l.back();
                auto cur = i.get()->getType();
                while (cur)
                {
                    is.push(cur);
                    cur = cur->getBase();
                }
            }
            
            InheritStruct& is = l.front();
            tod::Type* check_type = nullptr;
            auto greated_common_type = tod::Object::get_type();
            while (is.size())
            {
                check_type = is.top();
                for (auto& i : l)
                {
                    if (i.top() == check_type)
                    {
                        i.pop();
                        continue;
                    }
                    else return greated_common_type;
                }
                greated_common_type = check_type;
            }
            
            return greated_common_type;
        };
        auto type = gct(this->selections);
        if (nullptr == type) return;
        
        
        
        this->Freeze();
        this->GetSizer()->Clear(true);
        auto cur = type;
        while (cur)
        {
            PropertyListPanel* property_list_panel = new PropertyListPanel(this, cur, this->selections.front().get());
            this->GetSizer()->Add(property_list_panel, 0, wxEXPAND);
            cur = cur->getBase();
        }
        this->GetSizer()->Layout();
        this->Thaw();
    }
    
    
private:
    Selections selections;
};

class NodeHierarchy : public tod::EventDispatcher<wxWindow>
{
public:
    static const char* EVENT_SELECTED_NODE;
    enum
    {
        CONTEXTMENU_ADDNODE = 1,
        CONTEXTMENU_REMOVENODE,
        CONTEXTMENU_RENAME,
        CONTEXTMENU_SAVE,
        CONTEXTMENU_LOAD,
        CONTEXTMENU_EDITOR,
    };
    
public:
    NodeHierarchy(wxWindow* parent):EventDispatcher<wxWindow>(parent, -1)
    {
        auto sizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(sizer);
        
        //Node Hierarchy tree
        this->nodeTree = new wxTreeCtrl(this, -1, wxDefaultPosition,
                                        wxDefaultSize,
                                        wxBORDER_NONE | wxTR_EDIT_LABELS
                                        | wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT
                                        | wxTR_NO_LINES | wxTR_MULTIPLE);
        this->nodeTree->Bind(wxEVT_TREE_SEL_CHANGED, &NodeHierarchy::on_tree_sel_changed, this);
        this->nodeTree->Bind(wxEVT_TREE_ITEM_EXPANDING, &NodeHierarchy::on_tree_item_expanding, this);
        this->nodeTree->Bind(wxEVT_TREE_ITEM_EXPANDED, &NodeHierarchy::on_tree_item_expanded, this);
        this->nodeTree->Bind(wxEVT_TREE_ITEM_MENU, &NodeHierarchy::on_tree_item_menu, this);
        this->nodeTree->Bind(wxEVT_TREE_BEGIN_DRAG, &NodeHierarchy::on_tree_begin_drag, this);
        this->nodeTree->Bind(wxEVT_TREE_END_DRAG, &NodeHierarchy::on_tree_end_drag, this);
        this->nodeTree->SetBackgroundColour(TodEditorStyle::backgroundColor);
        this->nodeTree->SetForegroundColour(TodEditorStyle::normalFontColor);
        this->nodeTree->SetFont(TodEditorStyle::normalFont);
        this->nodeTree->SetIndent(5);
        sizer->Add(this->nodeTree, wxSizerFlags().Expand().Proportion(1));
        
        
        //Context Menu
        this->contextMenu = new wxMenu();
        this->contextMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, &NodeHierarchy::on_context_menu_selected, this);
        this->contextMenu->Append(new wxMenuItem(this->contextMenu, CONTEXTMENU_ADDNODE, "Add Node"));
        this->contextMenu->Append(new wxMenuItem(this->contextMenu, CONTEXTMENU_REMOVENODE, "Remove Node"));
        this->contextMenu->Append(new wxMenuItem(this->contextMenu, CONTEXTMENU_RENAME, "Rename Node"));
        this->contextMenu->AppendSeparator();
        this->contextMenu->Append(new wxMenuItem(this->contextMenu, CONTEXTMENU_SAVE, "Save"));
        this->contextMenu->Append(new wxMenuItem(this->contextMenu, CONTEXTMENU_LOAD, "Load"));
        this->contextMenu->AppendSeparator();
        this->contextMenu->Append(this->editorContextMenuItem = new wxMenuItem(this->contextMenu, CONTEXTMENU_EDITOR, "Editor"));
        
        
        tod::Kernel::instance()->create("Node", "/node1/node2/node3");
        
        this->setRoot(tod::Kernel::instance()->getRootNode());
    }
    void setRoot(tod::Node* root)
    {
        this->root = root;
        
        this->nodeTree->DeleteAllItems();
        TreeItemData* tree_item_data = new TreeItemData(this->root);
        wxTreeItemId root_tree_item_id = this->nodeTree->AddRoot("/", -1, -1, tree_item_data);
        if (this->root->getChildCount() == 0) return;
        this->nodeTree->SetItemHasChildren(root_tree_item_id);
    }
    void select(tod::Node* node)
    {
        this->nodeTree->SelectItem(this->nodeTree->GetRootItem());
    }
    Selections& getSelections()
    {
        return this->selections;
    }
    
private:
    class TreeItemData : public wxTreeItemData
    {
    public:
        TreeItemData(tod::Node* node):node(node) {}
        tod::ObjRef<tod::Node> node;
    };
    
    void on_tree_sel_changed(wxTreeEvent& e)
    {
        wxArrayTreeItemIds ids;
        size_t n = this->nodeTree->GetSelections(ids);
        if (n == 0) return;
        
        this->selections.clear();
        for (auto i : ids)
        {
            TreeItemData* tree_item_data = static_cast<TreeItemData*>(this->nodeTree->GetItemData(i));
            this->selections.push_back(tree_item_data->node);
        }
        this->dispatchEvent(EVENT_SELECTED_NODE);
    }
    void on_tree_item_expanding(wxTreeEvent& e)
    {
        TreeItemData* tree_item_data = static_cast<TreeItemData*>(this->nodeTree->GetItemData(e.GetItem()));
        if (tree_item_data->node->getChildCount() == 0) return;
        
        this->nodeTree->SetItemHasChildren(e.GetItem());
        this->nodeTree->DeleteChildren(e.GetItem());
        for (auto& i : tree_item_data->node->getChildren())
        {
            this->add_tree_item(i, e.GetItem());
        }
    }
    void on_tree_item_expanded(wxTreeEvent& e)
    {
    }
    void on_tree_item_menu(wxTreeEvent& e)
    {
        this->PopupMenu(this->contextMenu);
    }
    void on_tree_begin_drag(wxTreeEvent& e)
    {
        if (this->nodeTree->GetRootItem() == e.GetItem()) return;
        this->dragTreeItemId = e.GetItem();
        e.Allow();
    }
    void on_tree_end_drag(wxTreeEvent& e)
    {
        if (e.GetItem() == this->dragTreeItemId) return;
        TreeItemData* drop_source_tree_item_data = static_cast<TreeItemData*>(this->nodeTree->GetItemData(this->dragTreeItemId));
        TreeItemData* drop_target_tree_item_data = static_cast<TreeItemData*>(this->nodeTree->GetItemData(e.GetItem()));
        
        tod::Node* prev_parent_node = drop_source_tree_item_data->node->getParent();
        wxTreeItemId prev_parent_tree_item_id = this->nodeTree->GetItemParent(this->dragTreeItemId);
        
        this->add_tree_item(drop_source_tree_item_data->node, e.GetItem());
        this->nodeTree->Delete(this->dragTreeItemId);
        
        if (prev_parent_node && prev_parent_node->getChildCount() == 0)
        {
            this->nodeTree->SetItemHasChildren(prev_parent_tree_item_id, false);
        }
    }
    void on_context_menu_selected(wxCommandEvent& e)
    {
        wxArrayTreeItemIds selections;
        if (this->nodeTree->GetSelections(selections)==0) return;
        
        switch (e.GetId())
        {
            case CONTEXTMENU_ADDNODE:
            {
                for (auto s : selections)
                {
                    TreeItemData* tree_item_data = static_cast<TreeItemData*>(this->nodeTree->GetItemData(s));
                    //CreateNodeDialog* create_node_dialog = new CreateNodeDialog(this, tree_item_data->node);
                    //if (create_node_dialog->ShowModal() == wxID_OK)
                    {
                        /*this->add_tree_item(create_node_dialog->createdNode, selected_tree_item_id);
                         this->newNode = create_node_dialog->createdNode;
                         this->nodeTree->Expand(selected_tree_item_id);
                         this->inspector->setNode(create_node_dialog->createdNode);*/
                    }
                }
            }
                break;
            case CONTEXTMENU_REMOVENODE:
            {
                for (auto s : selections)
                {
                    this->remove_tree_item(s);
                }
            }
                break;
            case CONTEXTMENU_SAVE:
            {
                wxFileDialog* d = new wxFileDialog(this, "Save As");
                if (d->ShowModal() == wxID_OK)
                {
                    tod::Serializer s;
                    TreeItemData* tree_item_data = static_cast<TreeItemData*>(this->nodeTree->GetItemData(selections[0]));
                    //s.serializeToFile(tree_item_data->node, Serializer::PARSER_JSONCPP, d->GetPath().c_str());
                }
            }
                break;
            case CONTEXTMENU_LOAD:
            {
                wxFileDialog* d = new wxFileDialog(this, "Load");
                if (d->ShowModal() == wxID_OK)
                {
                    tod::Serializer s;
                    
                    TreeItemData* tree_item_data = nullptr;
                    for (auto i : selections)
                    {
                        tree_item_data = static_cast<TreeItemData*>(this->nodeTree->GetItemData(i));
                        auto uri = "file://" + d->GetPath();
                        tod::Node* load_node = s.deserializeFromJsonFile(uri.ToStdString());
                        if (nullptr==load_node) return;
                        tree_item_data->node->addChild(load_node);
                        
                        this->add_tree_item(load_node, i);
                        this->nodeTree->Expand(i);
                    }
                    
                    this->dispatchEvent(EVENT_SELECTED_NODE, tree_item_data->node);
                }
            }
                break;
            case CONTEXTMENU_EDITOR:
            {
                //TreeItemData* data = (TreeItemData*)this->nodeTree->GetItemData(selected_tree_item_id);
                //ToolExtensionMgr::instance()->openTool(data->node->getType()->name.c_str(), data->node);
            }
                break;
        }
    }
    
    wxTreeItemId add_tree_item(tod::Node* node, wxTreeItemId parent_tree_item_id)
    {
        if (!parent_tree_item_id.IsOk()) parent_tree_item_id = this->nodeTree->GetSelection();
        
        //append tree item
        TreeItemData* item_data = new TreeItemData(node);
        wxTreeItemId new_tree_item_id = this->nodeTree->AppendItem(parent_tree_item_id, node->getName(), -1, -1, item_data);
        this->nodeTree->SetItemHasChildren(parent_tree_item_id);
        
        //add + mark
        if (node->getChildCount() == 0) return new_tree_item_id;
        this->nodeTree->SetItemHasChildren(new_tree_item_id);
        
        return new_tree_item_id;
    }
    
    void remove_tree_item(wxTreeItemId selected_tree_item_id)
    {
        //remove from parent
        if (selected_tree_item_id == this->nodeTree->GetRootItem()) return;
        wxTreeItemId parent_tree_item_id = this->nodeTree->GetItemParent(selected_tree_item_id);
        TreeItemData* parent_data = static_cast<TreeItemData*>(this->nodeTree->GetItemData(parent_tree_item_id));
        TreeItemData* data = static_cast<TreeItemData*>(this->nodeTree->GetItemData(selected_tree_item_id));
        if (data->node->getParent()==0) return;
        data->node->removeFromParent();
        
        //delete tree item
        this->nodeTree->Delete(selected_tree_item_id);
        if (parent_data->node->getChildCount() == 0) this->nodeTree->SetItemHasChildren(parent_tree_item_id, false);
        
        //dispatch select node event
        this->dispatchEvent(EVENT_SELECTED_NODE);
    }
    
private:
    wxTreeCtrl* nodeTree;
    wxMenu* contextMenu;
    wxMenuItem* editorContextMenuItem;
    wxTreeItemId dragTreeItemId;
    tod::Node* root;
    Selections selections;
};

const char* NodeHierarchy::EVENT_SELECTED_NODE = "EVENT_SELECTED_NODE";

class MainFrame : public wxFrame
{
public:
    MainFrame():wxFrame(nullptr, -1, wxT("TodEditor"))
    {
        this->auiMgr.SetManagedWindow(this);
        this->auiMgr.SetArtProvider(new ArtProvider);
        
        //Node Hierarchy
        this->nodeHierarchy = new NodeHierarchy(this);
        this->auiMgr.AddPane(this->nodeHierarchy,
                             wxAuiPaneInfo()
                             .CloseButton()
                             .Left()
                             .MinSize(wxSize(250, 100))
                             .Caption(wxT("NodeHierarchy")));
        
        //Inspector
        this->inspector = new Inspector(this);
        this->nodeHierarchy->addEventHandler(
        NodeHierarchy::EVENT_SELECTED_NODE, this,
        [this](tod::Params*)
        {
            this->inspector->setSelections(
                this->nodeHierarchy->getSelections());
        });
        this->auiMgr.AddPane(this->inspector,
                             wxAuiPaneInfo()
                             .CloseButton()
                             .Right()
                             .MinSize(wxSize(250, 100))
                             .Caption(wxT("Inspector")));
        
        this->auiMgr.Update();
        
        
        
        this->nodeHierarchy->select(tod::Kernel::instance()->getRootNode());
    }
    virtual~MainFrame()
    {
        this->auiMgr.UnInit();
    }
    
private:
    class ArtProvider : public wxAuiDefaultDockArt
    {
    public:
        void DrawPaneButton(wxDC& dc,
                            wxWindow* window,
                            int button,
                            int buttonState,
                            const wxRect& rect,
                            wxAuiPaneInfo& pane) override
        {
        }
        
        void DrawSash(wxDC& dc,
                      wxWindow *window,
                      int orientation,
                      const wxRect& rect) override {}
        
        int GetMetric(int metricId) override
        {
            switch (metricId)
            {
                case wxAUI_DOCKART_PANE_BORDER_SIZE:
                    return 0;
                case wxAUI_DOCKART_SASH_SIZE:
                    return 1;
            }
            return wxAuiDefaultDockArt::GetMetric(metricId);
        }
    };
    
private:
    wxAuiManager auiMgr;
    NodeHierarchy* nodeHierarchy;
    Inspector* inspector;
    
};

class TodEditor : public wxApp
{
public:
    TodEditor() {}
    bool OnInit() override
    {
        MainFrame* frame = new MainFrame();
        this->SetTopWindow(frame);
        frame->Maximize();
        frame->Show();
        return true;
    }
};

wxDECLARE_APP(TodEditor);
wxIMPLEMENT_APP(TodEditor);


/*
int main(int argv, char* argc[])
{
    REGISTER_TYPE(tod::graphics::Renderer);
    
    auto renderer = static_cast<tod::graphics::Renderer*>
        (tod::Kernel::instance()->create("Renderer", "/sys/renderer"));
    renderer->initialize();
    return 0;
}
*/

/*int CALLBACK WinMain(
  _In_ HINSTANCE hInstance,
  _In_ HINSTANCE hPrevInstance,
  _In_ LPSTR     lpCmdLine,
  _In_ int       nCmdShow)
 {
	
	REGISTER_TYPE(tod::graphics::Renderer);
    
    auto renderer = static_cast<tod::graphics::Renderer*>
        (tod::Kernel::instance()->create("Renderer", "/sys/renderer"));
    renderer->initialize();
    return 0;
 }
 */





/*wxMenuBar* menubar = new wxMenuBar();
 wxMenu* testmenu = new  wxMenu();
 
 testmenu->Append(wxID_ANY, _("New\tCtrl-N"));
 testmenu->Append(wxID_ANY, _("Open\tCtrl-O"));
 
 testmenu->Append(wxID_ABOUT, _("About"));
 testmenu->Append(wxID_HELP, _("Help"));
 testmenu->Append(wxID_PREFERENCES, _("Preferences"));
 testmenu->Append(wxID_EXIT, _("Exit"));
 
 menubar->Append(testmenu, _("File"));
 
 frame->SetMenuBar(menubar);*/

