﻿//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------
#include "pch.h"

#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BINDING_DEBUG_OUTPUT
extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
#endif

#include "MainPage.xaml.h"

void ::BinaryModelConverter::MainPage::InitializeComponent()
{
    if (_contentLoaded)
    {
        return;
    }
    _contentLoaded = true;
    ::Windows::Foundation::Uri^ resourceLocator = ref new ::Windows::Foundation::Uri(L"ms-appx:///MainPage.xaml");
    ::Windows::UI::Xaml::Application::LoadComponent(this, resourceLocator, ::Windows::UI::Xaml::Controls::Primitives::ComponentResourceLocation::Application);
}

void ::BinaryModelConverter::MainPage::Connect(int __connectionId, ::Platform::Object^ __target)
{
    switch (__connectionId)
    {
    case 1:
        {
            this->NavView = safe_cast<::Windows::UI::Xaml::Controls::NavigationView^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::NavigationView^>(this->NavView))->SelectionChanged += ref new ::Windows::Foundation::TypedEventHandler<::Windows::UI::Xaml::Controls::NavigationView^, ::Windows::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs^>(this, (void (::BinaryModelConverter::MainPage::*)
                (::Windows::UI::Xaml::Controls::NavigationView^, ::Windows::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs^))&MainPage::NavView_SelectionChanged);
        }
        break;
    case 2:
        {
            this->AppTitle = safe_cast<::Windows::UI::Xaml::Controls::TextBlock^>(__target);
        }
        break;
    case 3:
        {
            this->ContentFrame = safe_cast<::Windows::UI::Xaml::Controls::Frame^>(__target);
        }
        break;
    }
    _contentLoaded = true;
}

::Windows::UI::Xaml::Markup::IComponentConnector^ ::BinaryModelConverter::MainPage::GetBindingConnector(int __connectionId, ::Platform::Object^ __target)
{
    __connectionId;         // unreferenced
    __target;               // unreferenced
    return nullptr;
}

#pragma warning(pop)


