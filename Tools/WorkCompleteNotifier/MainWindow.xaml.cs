using System.Windows;
using System.Windows.Input;

namespace WorkCompleteNotifier;

public partial class MainWindow : Window
{
    private bool _isClosing;

    public MainWindow(NotificationPayload payload)
    {
        InitializeComponent();
        DataContext = payload;
    }

    private void Window_Loaded(object sender, RoutedEventArgs e)
    {
        Activate();
        Focus();
    }

    private void CloseOnPreviewMouseDown(object sender, MouseButtonEventArgs e)
    {
        Dismiss();
    }

    private void CloseOnPreviewTouchDown(object sender, TouchEventArgs e)
    {
        Dismiss();
    }

    private void Dismiss()
    {
        if (_isClosing)
        {
            return;
        }

        _isClosing = true;
        Close();
    }
}
