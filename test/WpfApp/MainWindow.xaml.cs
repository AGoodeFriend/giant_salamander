using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace WpfApp
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        private DispatcherTimer getStatusTimer = null;

        public MainWindow()
        {
            InitializeComponent();




            Thread nThread = new Thread(() =>
            {
                ThreadPool.QueueUserWorkItem(o =>
                {
                    Application.Current.Dispatcher.BeginInvoke(DispatcherPriority.SystemIdle, new Action(() =>
                    {
                        getStatusTimer = new System.Windows.Threading.DispatcherTimer();
                        getStatusTimer.Tick += new EventHandler(getStatus);
                        getStatusTimer.Interval = new TimeSpan(0, 0, 0, 1);
                        getStatusTimer.Start();
                    }));
                });
            });
            nThread.Start();
        }

        private void getStatus(object sender, EventArgs e)
        {
            Console.WriteLine("hello..");
            Thread.Sleep(1000);
        }
    }
}
