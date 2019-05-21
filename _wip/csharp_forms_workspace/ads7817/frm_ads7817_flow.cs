using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace csharp_forms_workspace
{
    public partial class frm_ads7817_flow : Form
    {
        public frm_ads7817_flow()
        {
            InitializeComponent();


            // populate cmb_speed_hz

        }
//will need to maintain a list of slots.

        //private const int SLOT_QTY = 16;
        //private int tx_slot_index;
        //private int slot_max;
        //private int rx_slot_count;
        //private bool frame_stop_request;

        private void btn_add_slot_to_frame_Click(object sender, EventArgs e)
        {
            //int error_code;
            //adc_slot slot;
            //error_code = Ltc2449_Slot_Build(cb_slot_in_plus.SelectedIndex,  // positive channel
            //                                 cb_slot_in_minus.SelectedIndex, // negative channel
            //                                 cb_slot_oversample_rate.SelectedIndex, // over sample rate
            //                                 chk_slot_twox.CheckState,   // double speed
            //                                 tb_slot_delay.Text, // convert to delay in ns
            //                                 &slot    // return slot information.
            //                                );
            // if no error, then add this slot to the list
            // else report the issue.

            // post this human readable string to the outbound frame
            //string human_readable = Ltc2449_Slot_String(slot);

        }

        private void btn_cycle_manual_Click(object sender, EventArgs e)
        {
            //if (chk_cycle_frames.CheckState == true)
            //{
            //    error_code = Ltc2449_Frame_Resend(board, component_instance);
            //}
            //else
            //{
            //    error_code = Ltc2449_Slot_Send(board, component_instance, slot_max[tx_slot_index]);
            //    //increment tx_slot_index;
            //}
            
        }

        private void btn_cycle_start_Click(object sender, EventArgs e)
        {
            //if (chk_cycle_frames.CheckState == true)
            //{
            //    // commit slot list to library
            //    error_code = Ltc2449_Frame_Submit(board, component_instance, slot_list, slot_count);
            //}
            //else
            //{

            //}
        }




//TODO:        private uint32_t tx_frame[SLOT_QTY];
//TODO:        private int32_t rx_frame[SLOT_QTY];
#if false
        /*********************************************************************/
        public frm_Test_LTC2449()
        {
            InitializeComponent();


            // populate cmb_speed_hz

        }
        /*********************************************************************/
        private void btn_List_Add_Click(object sender, EventArgs e)
        {
            int error_code;
            int board;
            int index;

//TODO:              uint32_t op_code;

//TODO:              error_code = Ltc2449_Opcode_Build(  cmb_in_plus.SelectedValue, 
//TODO:                                                  cmb_in_minus.SelectedValue, 
//TODO:                                                  cmb_speed_hz.SelectedValue,
//TODO:                                                  &op_code );
//TODO:            if (error_code)
//TODO:            {
//TODO:                // bad op-code.  Post GUI status message.
//TODO:                return;
//TODO:            }

            // get the human readable version of the opcode
//TODO:              error_code = Ltc2449_Opcode_String(op_code, human_readable_op_code_string);

            // Insert op-code to GUI list at the currently highlighted position, otherwise
            //  add to the end of the list.  If the list is at SLOT_QTY, then do not add
            //  this slot, post a warning message in the status message box.

        }
        /*********************************************************************/
        private void btn_List_Clear_Click(object sender, EventArgs e)
        {
            // clear GUI list
        }
        /*********************************************************************/
        private void btn_List_Line_Remove_Click(object sender, EventArgs e)
        {
            // remove item from the GUI list. 
        }
        /*********************************************************************/
        private void btn_List_Commit_Start_Click(object sender, EventArgs e)
        {
            //disable:  Remove Slot, Frame Clear, Frame Start, Frame Save, Frame Load
            //disable: Manual, if frame-cycle not set to "Manual"
            //enable:  Frame Stop
            tx_slot_index = 0;
            slot_max   = list_Outbound_Opcode.Items.Count - 1;
        }
        /*********************************************************************/
        private void btn_List_Commit_Stop_Click(object sender, EventArgs e)
        {
            //Request that buffer commitments stop.
            frame_stop_request = true;
        }
        /*********************************************************************/
        private void Slot_Index_Increment()
        {
            if ( tx_slot_index >= slot_max ) tx_slot_index = 0;
            else tx_slot_index++;
        }
        /*********************************************************************/
        private void Frame_Stop()
        {
            frame_stop_request = false;
            //enable:  Remove Slot, Frame Clear, Frame Start, Frame Save, Frame Load
            //disable: Frame Stop
        }
        /*********************************************************************/
        private void Slot_Transmit()
        { /* driven by Manual button or timer */
            int error_code;

            string selectedItem = list_Outbound_Opcode.Items[tx_slot_index].ToString();
            //convert string to uint32_t by converting until basically a space is detected.
            UInt32 op_code = UInt32.Parse( selectedItem );
//TODO:              error_code = Ltc2449_Slot_Transmit( board, 0 /* component_instance */, op_code );

            if ( 0 == tx_slot_index )  
            {
                Frame_Receive();
                if ( frame_stop_request ) Frame_Stop();
            }
            Slot_Index_Increment();
        }
        /*********************************************************************/
        private void Frame_Receive()
        {
//TODO:            int error_code;
//TODO:            error_code = Ltc2449_Frame_Receive( board, component_instance, SLOT_QTY, rx_frame, &rx_slot_count );
            //post frame information to the GUI

        }

        private void btn_List_Commit_Manual_Click(object sender, EventArgs e)
        {
            Slot_Transmit();
        }
#endif
    }
}
