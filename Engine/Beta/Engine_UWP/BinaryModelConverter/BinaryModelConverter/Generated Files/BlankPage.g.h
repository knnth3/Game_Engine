﻿#pragma once
//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------


namespace Windows {
    namespace UI {
        namespace Xaml {
            namespace Controls {
                ref class Grid;
                ref class TextBlock;
                ref class Button;
            }
        }
    }
}

namespace BinaryModelConverter
{
    [::Windows::Foundation::Metadata::WebHostHidden]
    partial ref class BlankPage : public ::Windows::UI::Xaml::Controls::Page, 
        public ::Windows::UI::Xaml::Markup::IComponentConnector,
        public ::Windows::UI::Xaml::Markup::IComponentConnector2
    {
    public:
        void InitializeComponent();
        virtual void Connect(int connectionId, ::Platform::Object^ target);
        virtual ::Windows::UI::Xaml::Markup::IComponentConnector^ GetBindingConnector(int connectionId, ::Platform::Object^ target);
    
    private:
        void UnloadObject(::Windows::UI::Xaml::DependencyObject^ dependencyObject);
        void DisconnectUnloadedObject(int connectionId);
    
    private:
        bool _contentLoaded;
    
    
        private: ::Windows::UI::Xaml::Controls::Grid^ LayoutRoot;
        private: ::Windows::UI::Xaml::Controls::Grid^ Input;
        private: ::Windows::UI::Xaml::Controls::Grid^ Output;
        private: ::Windows::UI::Xaml::Controls::Grid^ Convert;
        private: ::Windows::UI::Xaml::Controls::Grid^ Spacing1;
        private: ::Windows::UI::Xaml::Controls::Grid^ Result1;
        private: ::Windows::UI::Xaml::Controls::Grid^ Result2;
        private: ::Windows::UI::Xaml::Controls::Grid^ Spacing2;
        private: ::Windows::UI::Xaml::Controls::Grid^ Log;
        private: ::Windows::UI::Xaml::Controls::TextBlock^ OutputTextBlock;
        private: ::Windows::UI::Xaml::Controls::TextBlock^ PickAFolderButtonResult;
        private: ::Windows::UI::Xaml::Controls::TextBlock^ PickAFileButtonResult;
        private: ::Windows::UI::Xaml::Controls::Button^ RunConversionButton;
        private: ::Windows::UI::Xaml::Controls::Button^ PickAFolderButton;
        private: ::Windows::UI::Xaml::Controls::Button^ PickAFileButton;
    };
}

