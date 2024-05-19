using System.Runtime.InteropServices;

namespace Demo
{
    public partial class DemoForm : Form
    {
        const int JSON_BUFFER_SIZE = 32768;

        [DllImport("SuitCLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static int call_suitc(ref IntPtr inputChars, int inputSize,
                                             ref IntPtr outputChars, ref int outputCharsSize,
                                             ref IntPtr optimumDetailsChars, ref int optimumDetailsCharSize);

        public DemoForm()
        {
            InitializeComponent();
            input.Text = "-wv9 -ev8 K3 2 AQJT987654";
        }

        private void buttonCalculate_Click(object sender, EventArgs e)
        {
            try
            {
                output.Text = Calculate(input.Text, out string optDetails);
                optimumDetails.Text = optDetails;
            }
            catch (Exception ex)
            {
                output.Text = ex.Message;
            }
        }

        private string Calculate(string inputString, out String optimumDetails)
        {
            optimumDetails = "";
            
            // Input chars
            char[] inputCharArray = new char[inputString.Length];
            for (int i = 0; i < inputString.Length; i++)
            {
                inputCharArray[i] = inputString[i];
            }



            int charSize = 2;

            int size = charSize * inputString.Length;
            IntPtr inputCharsBuffer = Marshal.AllocCoTaskMem(size);
            Marshal.Copy(inputCharArray, 0, inputCharsBuffer, inputString.Length);

            //Output
            int bufferLen = JSON_BUFFER_SIZE;
            int outputCharsSize = 0;
            IntPtr outputCharsBuffer = Marshal.AllocCoTaskMem(charSize * bufferLen);

            //OptimumDetails
            int optimumDetailsBufferLen = JSON_BUFFER_SIZE;
            int optimumDetailsCharsSize = 0;
            IntPtr optimumDetailsCharsBuffer = Marshal.AllocCoTaskMem(charSize * optimumDetailsBufferLen);

            int result = call_suitc(ref inputCharsBuffer, inputString.Length,
                                    ref outputCharsBuffer, ref outputCharsSize,
                                    ref optimumDetailsCharsBuffer, ref optimumDetailsCharsSize);

            String s = "";
            if (result == 0)
            {
                if (outputCharsSize > 0)
                {
                    char[] outputChars = new char[outputCharsSize];
                    Marshal.Copy(outputCharsBuffer, outputChars, 0, outputCharsSize);
                    foreach (char c in outputChars)
                    {
                        s += c;
                    }
                    if (optimumDetailsCharsSize > 0)
                    {
                        optimumDetails = "";
                        char[] optimumDetailsChars = new char[optimumDetailsCharsSize];
                        Marshal.Copy(optimumDetailsCharsBuffer, optimumDetailsChars, 0, optimumDetailsCharsSize);
                        foreach (char c in optimumDetailsChars)
                        {
                            optimumDetails += c;
                        }
                    }
                }
                else
                {
                    s = "Error: No result returned";
                }
            }
            else
            {
                s = "Error: " + result.ToString();
            }

            Marshal.FreeCoTaskMem(optimumDetailsCharsBuffer);
            Marshal.FreeCoTaskMem(inputCharsBuffer);
            Marshal.FreeCoTaskMem(outputCharsBuffer);



            return s;

        }
    }
}
