using Newtonsoft.Json;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Xml;
using Formatting = Newtonsoft.Json.Formatting;

namespace Demo
{
    public partial class DemoForm : Form
    {
        // The library writes up to JSON_BUFFER_SIZE (32768*4) characters plus a
        // terminating null into each output buffer, without checking the size.
        const int JSON_BUFFER_SIZE = 32768 * 4 + 1;

        [DllImport("SuitCLib.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static int call_suitc(ref IntPtr inputChars, int inputSize,
                                             ref IntPtr outputChars, ref int outputCharsSize,
                                             ref IntPtr optimumDetailsChars, ref int optimumDetailsCharSize);

        public DemoForm()
        {
            InitializeComponent();
            input.Text = "-wv9 -ev8 K3 2 AQJT987654";
        }

        private void ApplySyntaxHighlighting()
        {
            output.SelectAll();
            output.SelectionColor = Color.Black; // Default color

            // Color JSON keys (e.g., "name":)
            HighlightPattern("\"(.*?)\"\\s*:", Color.Blue);

            // Color string values (e.g., "New York")
            HighlightPattern(":\\s*\"(.*?)\"", Color.Brown);

            // Color numbers (e.g., 52.9412, 1, 0.5294)
            HighlightPattern("\\b\\d+(\\.\\d+)?\\b", Color.Green);
        }

        private void HighlightPattern(string pattern, Color color)
        {
            MatchCollection matches = Regex.Matches(output.Text, pattern);
            foreach (Match match in matches)
            {
                output.Select(match.Index, match.Length);
                output.SelectionColor = color;
            }
        }
        private void buttonCalculate_Click(object sender, EventArgs e)
        {
            try
            {
                
                var jsonString = Calculate(input.Text, out string optDetails);
                string formattedJson = JsonConvert.SerializeObject(JsonConvert.DeserializeObject(jsonString), Formatting.Indented);
                output.Text = formattedJson;
                ApplySyntaxHighlighting();
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

            // On success the output holds the analysis; on failure (result 1) it
            // holds a JSON document with the error message.
            string s = outputCharsSize > 0
                ? Marshal.PtrToStringUni(outputCharsBuffer, outputCharsSize)
                : "Error: No result returned";
            if (result == 0 && optimumDetailsCharsSize > 0)
            {
                optimumDetails = Marshal.PtrToStringUni(optimumDetailsCharsBuffer, optimumDetailsCharsSize);
            }

            Marshal.FreeCoTaskMem(optimumDetailsCharsBuffer);
            Marshal.FreeCoTaskMem(inputCharsBuffer);
            Marshal.FreeCoTaskMem(outputCharsBuffer);



            return s;

        }
    }
}
