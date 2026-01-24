#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include <msclr\lock.h>

namespace MeshMakerCppCLI
{
	public ref class Invocation
	{
	private:
		Delegate^ _action;
		array<Object^>^ _arguments;
	public:
		Invocation(Delegate^ action, ... array<Object^>^ arguments)
		{
			_action = action;
			_arguments = arguments;
		}

		void Perform()
		{
			_action->DynamicInvoke(_arguments);
		}
	};

	public ref class UndoableAction
	{
	private:
		String^ name;
		Invocation^ invocation;

	public:
		property String^ Name
		{
			String^ get() { return name; }
		}

		void Perform()
		{
			invocation->Perform();
		}

		UndoableAction(String^ name, Invocation^ invocation)
		{
			this->name = name;
			this->invocation = invocation;
		}
	};

	public ref class UndoManager
	{
	private:
		Stack<UndoableAction^>^ undoStack;
		Stack<UndoableAction^>^ redoStack;
		bool undoing;
		int lastUndoStackCount;

	public:
		event EventHandler^ NeedsSaveChanged;

		UndoManager()
		{
			undoStack = gcnew Stack<UndoableAction^>();
			redoStack = gcnew Stack<UndoableAction^>();
			undoing = false;
			lastUndoStackCount = 0;
		}

		property bool NeedsSave
		{
			bool get()
			{
				if (CanUndo && lastUndoStackCount != undoStack->Count)
					return true;
				return false;
			}
		}

		property bool CanUndo
		{
			bool get() { return undoStack->Count > 0; }
		}

		property bool CanRedo
		{
			bool get() { return redoStack->Count > 0; }
		}

		property String^ UndoName
		{
			String^ get() { return undoStack->Peek()->Name; }
		}

		property String^ RedoName
		{
			String^ get() { return redoStack->Peek()->Name; }
		}

		void PrepareUndo(String^ name, Invocation^ invocation)
		{
			UndoableAction^ undoableAction = gcnew UndoableAction(name, invocation);
			if (undoing)
				redoStack->Push(undoableAction);
			else
				undoStack->Push(undoableAction);
			OnNeedsSaveChanged();
		}

		void Undo()
		{
			if (CanUndo)
			{
				UndoableAction^ undoableAction = undoStack->Pop();
				undoing = true;
				undoableAction->Perform();
				undoing = false;
				OnNeedsSaveChanged();
			}
		}

		void Redo()
		{
			if (CanRedo)
			{
				UndoableAction^ undoableAction = redoStack->Pop();
				undoableAction->Perform();
				OnNeedsSaveChanged();
			}
		}

		void Clear()
		{
			undoStack->Clear();
			redoStack->Clear();
			undoing = false;
			lastUndoStackCount = undoStack->Count;
			OnNeedsSaveChanged();
		}

		void DocumentSaved()
		{
			lastUndoStackCount = undoStack->Count;
			OnNeedsSaveChanged();
		}

		void OnNeedsSaveChanged()
		{
			NeedsSaveChanged(this, EventArgs::Empty);
		}
	};

	public ref class RunningVisualStudio
	{
	private:
		bool _IsRunningVisualStudio = false;
	public:
		property bool IsRunningVisualStudio
		{
			bool get() { return _IsRunningVisualStudio; }
		}

		RunningVisualStudio()
		{
			Process^ currentProcess = Process::GetCurrentProcess();
			if (String::Equals(currentProcess->ProcessName->ToLowerInvariant(), "devenv")) //!-!
				_IsRunningVisualStudio = true;
			else
				_IsRunningVisualStudio = false;
		}
	};

	public ref class Tools abstract sealed
	{
	private:
		static RunningVisualStudio^ _runningVS = nullptr;
		static Object^ _lock = gcnew Object();

	public:
		property static RunningVisualStudio^ RunningVS
		{
			RunningVisualStudio ^ get()
			{
				if (_runningVS != nullptr)
					return _runningVS;

				{
					msclr::lock lock(_lock);
					if (_runningVS == nullptr)
						_runningVS = gcnew RunningVisualStudio();
					return _runningVS;
				}
			}
		}

		property static bool SafeDesignMode
		{
			bool get()
			{
				return RunningVS->IsRunningVisualStudio;
			}
		}

	};
}
