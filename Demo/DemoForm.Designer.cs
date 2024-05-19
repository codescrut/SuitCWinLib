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
            output = new TextBox();
            buttonCalculate = new Button();
            optimumDetails = new TextBox();
            label3 = new Label();
            SuspendLayout();
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(35, 19);
            label1.Name = "label1";
            label1.Size = new Size(35, 15);
            label1.TabIndex = 0;
            label1.Text = "Input";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(35, 164);
            label2.Name = "label2";
            label2.Size = new Size(45, 15);
            label2.TabIndex = 1;
            label2.Text = "Output";
            // 
            // input
            // 
            input.Location = new Point(35, 52);
            input.Multiline = true;
            input.Name = "input";
            input.Size = new Size(672, 95);
            input.TabIndex = 2;
            // 
            // output
            // 
            output.Location = new Point(35, 194);
            output.Multiline = true;
            output.Name = "output";
            output.Size = new Size(672, 640);
            output.TabIndex = 3;
            // 
            // buttonCalculate
            // 
            buttonCalculate.Location = new Point(741, 91);
            buttonCalculate.Name = "buttonCalculate";
            buttonCalculate.Size = new Size(75, 23);
            buttonCalculate.TabIndex = 4;
            buttonCalculate.Text = "Calculate";
            buttonCalculate.UseVisualStyleBackColor = true;
            buttonCalculate.Click += buttonCalculate_Click;
            // 
            // optimumDetails
            // 
            optimumDetails.Location = new Point(741, 194);
            optimumDetails.Multiline = true;
            optimumDetails.Name = "optimumDetails";
            optimumDetails.Size = new Size(672, 640);
            optimumDetails.TabIndex = 6;
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(741, 164);
            label3.Name = "label3";
            label3.Size = new Size(96, 15);
            label3.TabIndex = 5;
            label3.Text = "Optimum details";
            // 
            // DemoForm
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1467, 861);
            Controls.Add(optimumDetails);
            Controls.Add(label3);
            Controls.Add(buttonCalculate);
            Controls.Add(output);
            Controls.Add(input);
            Controls.Add(label2);
            Controls.Add(label1);
            Name = "DemoForm";
            Text = "SuitCLib Demo";
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label label1;
        private Label label2;
        private TextBox input;
        private TextBox output;
        private Button buttonCalculate;
        private TextBox optimumDetails;
        private Label label3;
    }
}
