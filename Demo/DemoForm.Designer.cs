namespace Demo
{
    partial class DemoForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            label1 = new Label();
            label2 = new Label();
            input = new TextBox();
            output = new RichTextBox();
            buttonCalculate = new Button();
            optimumDetails = new RichTextBox();
            label3 = new Label();
            SuspendLayout();
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(50, 32);
            label1.Margin = new Padding(4, 0, 4, 0);
            label1.Name = "label1";
            label1.Size = new Size(54, 25);
            label1.TabIndex = 0;
            label1.Text = "Input";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(50, 174);
            label2.Margin = new Padding(4, 0, 4, 0);
            label2.Name = "label2";
            label2.Size = new Size(69, 25);
            label2.TabIndex = 1;
            label2.Text = "Output";
            // 
            // input
            // 
            input.Location = new Point(50, 87);
            input.Margin = new Padding(4, 5, 4, 5);
            input.Multiline = true;
            input.Name = "input";
            input.Size = new Size(958, 41);
            input.TabIndex = 2;
            // 
            // output
            // 
            output.Location = new Point(50, 215);
            output.Margin = new Padding(4, 5, 4, 5);
            output.Name = "output";
            output.ScrollBars = RichTextBoxScrollBars.ForcedBoth;
            output.ShowSelectionMargin = true;
            output.Size = new Size(958, 1064);
            output.TabIndex = 3;
            output.Text = "";
            output.WordWrap = false;
            // 
            // buttonCalculate
            // 
            buttonCalculate.Location = new Point(1059, 90);
            buttonCalculate.Margin = new Padding(4, 5, 4, 5);
            buttonCalculate.Name = "buttonCalculate";
            buttonCalculate.Size = new Size(107, 38);
            buttonCalculate.TabIndex = 4;
            buttonCalculate.Text = "Calculate";
            buttonCalculate.UseVisualStyleBackColor = true;
            buttonCalculate.Click += buttonCalculate_Click;
            // 
            // optimumDetails
            // 
            optimumDetails.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            optimumDetails.Font = new Font("Courier New", 10F, FontStyle.Regular, GraphicsUnit.Point, 0);
            optimumDetails.Location = new Point(1059, 218);
            optimumDetails.Margin = new Padding(4, 5, 4, 5);
            optimumDetails.Name = "optimumDetails";
            optimumDetails.ScrollBars = RichTextBoxScrollBars.ForcedBoth;
            optimumDetails.Size = new Size(958, 1064);
            optimumDetails.TabIndex = 6;
            optimumDetails.Text = "";
            optimumDetails.WordWrap = false;
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(1059, 174);
            label3.Margin = new Padding(4, 0, 4, 0);
            label3.Name = "label3";
            label3.Size = new Size(145, 25);
            label3.TabIndex = 5;
            label3.Text = "Optimum details";
            // 
            // DemoForm
            // 
            AutoScaleDimensions = new SizeF(10F, 25F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(2096, 1435);
            Controls.Add(optimumDetails);
            Controls.Add(label3);
            Controls.Add(buttonCalculate);
            Controls.Add(output);
            Controls.Add(input);
            Controls.Add(label2);
            Controls.Add(label1);
            Margin = new Padding(4, 5, 4, 5);
            Name = "DemoForm";
            Text = "SuitCLib Demo";
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label label1;
        private Label label2;
        private TextBox input;
        private RichTextBox output;
        private Button buttonCalculate;
        private RichTextBox optimumDetails;
        private Label label3;
    }
}
