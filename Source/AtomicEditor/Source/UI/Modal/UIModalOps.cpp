// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_message_window.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/UI/TBUI.h>

#include "AEEvents.h"

#include "Resources/AEResourceOps.h"

#include "UI/UIMainFrame.h"
#include "UI/UIProjectFrame.h"

#include "UIModalOps.h"
#include "UIResourceOps.h"
#include "UIBuild.h"
#include "UIBuildSettings.h"
#include "UIProgramOutput.h"
#include "UINewProject.h"
#include "UIAbout.h"
#include "UIPlatformsInfo.h"

#include "License/UIActivation.h"
#include "License/UIActivationSuccess.h"
#include "License/UIManageLicense.h"
#include "License/AELicenseSystem.h"
#include "License/UIEulaAgreement.h"

namespace AtomicEditor
{

// UIModalOpWindow------------------------------------------------

UIModalOpWindow::UIModalOpWindow(Context* context):
    AEWidget(context)
{
    window_ = new TBWindow();
    TBUI* tbui = GetSubsystem<TBUI>();
    TBWidget* root = tbui->GetRootWidget();
    root->AddChild(delegate_);
    // start with full screen as size
    delegate_->SetRect(TBRect(0, 0, root->GetRect().w, root->GetRect().h));
    delegate_->AddChild(window_);
}

UIModalOpWindow::~UIModalOpWindow()
{
}

bool UIModalOpWindow::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}

void UIModalOpWindow::Center()
{
    TBUI* tbui = GetSubsystem<TBUI>();
    TBRect rect = window_->GetRect();
    TBWidget* root = tbui->GetRootWidget();
    TBRect bounds(0, 0, root->GetRect().w, root->GetRect().h);
    window_->SetRect(rect.CenterIn(bounds).MoveIn(bounds).Clip(bounds));
    delegate_->SetRect(bounds);
}

// UIModalOps------------------------------------------------

UIModalOps::UIModalOps(Context* context) :
    AEWidget(context),
    dimmer_(0),
    isHiding_(false)
{
    TBWidgetListener::AddGlobalListener(this);
    context->RegisterSubsystem(this);
    dimmer_ = new TBDimmer();
}

void UIModalOps::Show()
{
    assert(!dimmer_->GetParent());
    TBUI* tbui = GetSubsystem<TBUI>();
    TBWidget* root = tbui->GetRootWidget();
    root->AddChild(dimmer_);
}

void UIModalOps::Hide()
{
    isHiding_ = true;

    if (dimmer_->GetParent())
        dimmer_->GetParent()->RemoveChild(dimmer_);

    opWindow_ = NULL;

    isHiding_ = false;

}

void UIModalOps::ShowCreateComponent(const String& resourcePath)
{
    assert(opWindow_.Null());

    resourcePath_ = resourcePath;
    Show();
    opWindow_ = new UICreateComponent(context_);
}


void UIModalOps::ShowCreateScript(const String& resourcePath)
{
    assert(opWindow_.Null());

    resourcePath_ = resourcePath;
    Show();
    opWindow_ = new UICreateScript(context_);
}

void UIModalOps::ShowResourceDelete(const String& resourcePath)
{
    assert(opWindow_.Null());

    resourcePath_ = resourcePath;
    Show();
    opWindow_ = new UIResourceDelete(context_);
}

void UIModalOps::ShowNewFolder(const String& resourcePath)
{
    assert(opWindow_.Null());

    resourcePath_ = resourcePath;
    Show();
    opWindow_ = new UINewFolder(context_);
}

void UIModalOps::ShowBuildSettings()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIBuildSettings(context_);
}

void UIModalOps::ShowBuild()
{
    assert(opWindow_.Null());

    Show();
// BEGIN LICENSE MANAGEMENT
    LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();
    if (licenseSystem->HasPlatformLicense())
        opWindow_ = new UIBuild(context_);
    else
        opWindow_ = new PlatformsInfo(context_);
// END LICENSE MANAGEMENT
}


void UIModalOps::ShowNewProject()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UINewProject(context_);
}

void UIModalOps::SetProgramOutputSubprocess(Object* subprocess)
{
    if (opWindow_.Null())
        return;

    if (opWindow_->GetType() != UIProgramOutput::GetTypeStatic())
        return;

    UIProgramOutput* output = (UIProgramOutput*)(opWindow_.Get());
    output->SetSubprocess(subprocess);
}

void UIModalOps::ShowProgramOutput(Object *subprocess)
{
    assert(opWindow_.Null());

    Show();
    UIProgramOutput* output = new UIProgramOutput(context_);
    output->SetSubprocess(subprocess);
    opWindow_ = output;

}

void UIModalOps::ShowActivation()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIActivation(context_);

}

void UIModalOps::ShowActivationSuccess()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIActivationSuccess(context_);

}

void UIModalOps::ShowAbout()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIAbout(context_);

}

void UIModalOps::ShowManageLicense()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIManageLicense(context_);

}

void UIModalOps::ShowPlatformsInfo()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new PlatformsInfo(context_);
}

void UIModalOps::ShowEulaAgreement()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIEulaAgreement(context_);

}

void UIModalOps::OnWidgetDelete(TBWidget *widget)
{
    if (isHiding_)
        return;

    if (opWindow_)
    {
        if (widget == opWindow_->GetWindow())
            Hide();
    }

}

bool UIModalOps::OnWidgetDying(TBWidget *widget)
{
    return false;
}

UIModalOps::~UIModalOps()
{

}

bool UIModalOps::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}

}