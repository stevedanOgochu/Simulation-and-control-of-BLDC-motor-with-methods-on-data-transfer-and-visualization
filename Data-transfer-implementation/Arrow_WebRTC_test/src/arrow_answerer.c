// Copyright Stevedan Ogochukwu Omodolor 17/05/2020 Ku leuven UPC EEBE
// Project: Bachelor thesis- Simulaton and control of a BLDC motor with methods on data transfer and visualitzation
// Tutor : Herman Bruyninckx

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//  @file arrow_answerer.c
// @brief Implementation of arrow_answerer
//
// This file is the implementation of the arrow_answerer. it is responsible for displaying the data in arrow and format and diplay
// that data to console. This example requires the sdp protocol and canditate data be copied manually to the console to make the initial
// connection and open the datachannel.
//


#include "arrow_answerer.h"




Peer *peer = NULL;
int number_of_rows = 0;
int main(int argc, char **argv) {


	// configuration peer to peer connection
        rtcInitLogger(RTC_LOG_DEBUG);

        // Create peer
        rtcConfiguration config;
        memset(&config, 0, sizeof(config));

        Peer *peer = (Peer *)malloc(sizeof(Peer));
        if (!peer) {

                printf("Error allocating memory for peer\n");
                deletePeer(peer);

        }
        memset(peer, 0, sizeof(Peer));

        printf("Peer created\n");

        // Create peer connection
        peer->pc = rtcCreatePeerConnection(&config);

        rtcSetUserPointer(peer->pc, peer);
        rtcSetLocalDescriptionCallback(peer->pc, descriptionCallback);
        rtcSetLocalCandidateCallback(peer->pc, candidateCallback);
        rtcSetStateChangeCallback(peer->pc, stateChangeCallback);
        rtcSetGatheringStateChangeCallback(peer->pc, gatheringStateCallback);
        rtcSetUserPointer(peer->dc, NULL);
        rtcSetDataChannelCallback(peer->pc, dataChannelCallback);




        bool exit = false;

        while (!exit) {

                printf("\n");
                printf("***************************************************************************************\n");

                // << endl
                printf("* 0: Exit /"
                       " 1: Enter remote description /"
                       " 2: Enter remote candidate\n"
                       "[Command]: ");

                int command = -1;
                int c;
                // int check_scan
                if (scanf("%d", &command)) {

                }else {
                        break;
                }
                while ((c = getchar()) != '\n' && c != EOF) { }

                fflush(stdin);
                switch (command) {
                case 0: {
                        exit = true;
                        break;
                }
                case 1: {
                        // Parse Description
                        printf("[Description]: ");


                        char *line = NULL;
                        size_t len = 0;
                        size_t read = 0;
                        char *sdp = (char*) malloc(sizeof(char));
                        while ((read = getline(&line, &len, stdin)) != -1 && !all_space(line)) {
                                sdp = (char*) realloc (sdp,(strlen(sdp)+1) +strlen(line)+1);
                                strcat(sdp, line);

                        }
                        printf("%s\n",sdp);
                        rtcSetRemoteDescription(peer->pc, sdp, "offer");
                        free(sdp);
                        free(line);
                        break;

                }
                case 2: {
                        // Parse Candidate
                        printf("[Candidate]: ");
                        char* candidate = NULL;
                        size_t candidate_size = 0;

                        if(getline(&candidate, &candidate_size, stdin)) {
                                rtcAddRemoteCandidate(peer->pc, candidate, "0");
                                free(candidate);

                        }else {
                                printf("Error reading line\n");
                                break;
                        }

                        break;
                }
                default: {
                        printf("** Invalid Command **");
                        break;
                }
																// if(!peer->connected || peer->state != RTC_CONNECTED ) {
																// 								exit = true;
                //
																// }
                }
        }


        deletePeer(peer);
								// free(data);
        return 0;
}


static void descriptionCallback(const char *sdp, const char *type, void *ptr) {
        // Peer *peer = (Peer *)ptr;
        printf("Description %s:\n%s\n", "answerer", sdp);
}

static void candidateCallback(const char *cand, const char *mid, void *ptr) {
        // Peer *peer = (Peer *)ptr;
        printf("Candidate %s: %s\n", "answerer", cand);

}

static void stateChangeCallback(rtcState state, void *ptr) {
        Peer *peer = (Peer *)ptr;
        peer->state = state;
        printf("State %s: %s\n", "answerer", state_print(state));
}

static void gatheringStateCallback(rtcGatheringState state, void *ptr) {
        Peer *peer = (Peer *)ptr;
        peer->gatheringState = state;
        printf("Gathering state %s: %s\n", "answerer", rtcGatheringState_print(state));
}



static void closedCallback(void *ptr) {
        Peer *peer = (Peer *)ptr;
        peer->connected = false;


}
static void messageCallback(const char *message, int size, void *ptr) {
        // Peer *peer = (Peer *)ptr;
								gconstpointer data_input = (gconstpointer )message;
								gsize data_size = (gsize ) size;
								//display buffer
								 display_arrow_buffer(data_input, data_size);
        // if (size < 0) { // negative size indicates a null-terminated string
        //         printf("Message %s: %s\n", "answerer", message);
        // } else {
        //         printf("Message %s: [binary of size %d]\n", "answerer", size);
        // }

}
static void deletePeer(Peer *peer) {
        if (peer) {
                if (peer->dc)
                        rtcDeleteDataChannel(peer->dc);
                if (peer->pc)
                        rtcDeletePeerConnection(peer->pc);
                free(peer);
        }
}

static void dataChannelCallback(int dc, void *ptr) {
        Peer *peer = (Peer *)ptr;
        peer->dc = dc;
        peer->connected = true;
        rtcSetClosedCallback(dc, closedCallback);
        rtcSetMessageCallback(dc, messageCallback);
        char buffer[256];
        if (rtcGetDataChannelLabel(dc, buffer, 256) >= 0)
                printf("DataChannel %s: Received with label \"%s\"\n", "answerer", buffer);


}
int all_space(const char *str) {
        while (*str) {
                if (!isspace(*str++)) {
                        return 0;
                }
        }
        return 1;
}

char* state_print(rtcState state) {
        char *str = NULL;
        switch (state) {
        case RTC_NEW:
                str = "RTC_NEW";
                break;
        case RTC_CONNECTING:
                str = "RTC_CONNECTING";
                break;
        case RTC_CONNECTED:
                str = "RTC_CONNECTED";
                break;
        case RTC_DISCONNECTED:
                str = "RTC_DISCONNECTED";
                break;
        case RTC_FAILED:
                str = "RTC_FAILED";
                break;
        case RTC_CLOSED:
                str = "RTC_CLOSED";
                break;
        default:
                break;
        }

        return str;

}

char* rtcGatheringState_print(rtcState state) {
        char* str = NULL;
        switch (state) {
        case RTC_GATHERING_NEW:
                str = "RTC_GATHERING_NEW";
                break;
        case RTC_GATHERING_INPROGRESS:
                str = "RTC_GATHERING_INPROGRESS";
                break;
        case RTC_GATHERING_COMPLETE:
                str = "RTC_GATHERING_COMPLETE";
                break;
        default:
                break;
        }

        return str;

}



static void print_record_batch(GArrowRecordBatch *record_batch)
{
        guint nth_column, n_columns;
        gint64 n_rows, nth_row;
        n_columns = garrow_record_batch_get_n_columns(record_batch);
        n_rows = garrow_record_batch_get_n_rows(record_batch);
        GArrowArray *array[n_columns];
        const gchar * column_name[n_columns];

        // getting the array of each column
        for (nth_column = 0; nth_column < n_columns; nth_column++) {
                array[nth_column] = garrow_record_batch_get_column_data(record_batch, nth_column);
                column_name[nth_column] = garrow_record_batch_get_column_name(record_batch, nth_column);
                //
                // g_print("columns[%u](%s): ",
                // nth_column,
                // garrow_record_batch_get_column_name(record_batch, nth_column));
                // print_array(array);
                // g_object_unref(array);
        }

        // print data row by row
        for(nth_row = 0; nth_row < n_rows; nth_row++) {
                sleep(0.5);
                g_print("row: %d", number_of_rows);
                for (nth_column = 0; nth_column < n_columns; nth_column++) {
                        g_print("[%s]: %g\t", column_name[nth_column], garrow_float_array_get_value (GARROW_FLOAT_ARRAY(array[nth_column]),
                                                                                            nth_row));
                }

                g_print("\n");
                number_of_rows++;
        }

        // unrefereing array table
        for (nth_column = 0; nth_column < n_columns; nth_column++) {
                g_object_unref(array[nth_column]);
        }


}

void display_arrow_buffer(gconstpointer data_input, gsize data_size)
{



        GBytes * data = g_bytes_new(data_input, data_size);
        // Passing the data into the buffer
        GArrowBuffer *buffer = garrow_buffer_new_bytes (data);

        // input stream
        GArrowBufferInputStream * input = garrow_buffer_input_stream_new (buffer);

        GError *error = NULL;


        {
                GArrowRecordBatchReader *reader;
                GArrowRecordBatchStreamReader *stream_reader;

                stream_reader =
                        garrow_record_batch_stream_reader_new(GARROW_INPUT_STREAM(input),
                                                              &error);
                if (!stream_reader) {
                        g_print("failed to open stream reader: %s\n", error->message);
                        g_error_free(error);
                        g_object_unref(input);
                        return;
                }

                reader = GARROW_RECORD_BATCH_READER(stream_reader);
                while (TRUE) {
                        GArrowRecordBatch *record_batch;

                        record_batch = garrow_record_batch_reader_read_next(reader, &error);
                        if (error) {
                                g_print("failed to read the next record batch: %s\n", error->message);
                                g_error_free(error);
                                g_object_unref(reader);
                                g_object_unref(input);
                                return;
                        }

                        if (!record_batch) {
                                break;
                        }

                        print_record_batch(record_batch);
                        g_object_unref(record_batch);
                }

                g_object_unref(reader);
        }

        g_object_unref(input);
        g_object_unref(buffer);
        g_bytes_unref(data);
}
