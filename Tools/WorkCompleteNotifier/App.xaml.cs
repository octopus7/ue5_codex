using System.Windows;

namespace WorkCompleteNotifier;

public partial class App : Application
{
    protected override void OnStartup(StartupEventArgs e)
    {
        base.OnStartup(e);

        var payload = NotificationPayload.FromArguments(e.Args);
        var window = new MainWindow(payload);

        MainWindow = window;
        window.Show();
    }
}
