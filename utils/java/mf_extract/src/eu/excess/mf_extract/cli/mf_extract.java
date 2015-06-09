// mf_extract.java
//    Copyright (C) 2015  Anders Gidenstam  (anders(at)gidenstam.org)
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

package eu.excess.mf_extract.cli;

import java.io.*;

import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONTokener;

public class mf_extract
{
    private static String TIMESTAMP = "Timestamp";

    private static String LIKWID_SOCKET0_PKG = "likwid::socket0::PKG::energy";
    private static String LIKWID_SOCKET1_PKG = "likwid::socket1::PKG::energy";
    private static String LIKWID_SOCKET0_PP0 = "likwid::socket0::PP0::energy";
    private static String LIKWID_SOCKET1_PP0 = "likwid::socket1::PP0::energy";
    private static String LIKWID_SOCKET0_DRAM = "likwid::socket0::DRAM::energy";
    private static String LIKWID_SOCKET1_DRAM = "likwid::socket1::DRAM::energy";

    private static String EXTERNAL_SOCKET0 = "CPU1_node01";
    private static String EXTERNAL_SOCKET1 = "CPU2_node01";
    private static String EXTERNAL_GPU0    = "GPU1_node01";
    private static String EXTERNAL_ATX12V  = "ATX12V_node01";

    JSONArray mfEvents = new JSONArray();
    int processingType = 0;

    public mf_extract()
    {
    }

    public void run(String[] args)
        throws IOException
    {
        processArguments(args);
        switch (processingType) {
        case 0:
            processLikwidEvents(mfEvents);
            break;
        case 1:
            processExternalPowerEvents(mfEvents);
            break;
        }
    }

    private void processArguments(String[] args)
        throws IOException
    {
        if (args.length < 1) {
            printUsage();
            System.exit(-1);
        } else {
            for (int i = 0; i < args.length; i++) {
                if (args[i].equals("-h")) {
                    printUsage();
                    System.exit(-1);
                } else if (args[i].equals("-likwid")) {
                    processingType = 0;
                } else if (args[i].equals("-external")) {
                    processingType = 1;
                } else {
                    mfEvents = loadMFEvents(args[i]);
                }
            }
        }
    }

    private void printUsage()
    {
        System.out.println
            ("Usage: mf_extract [options] <JSON formatted data file>");
        System.out.println();
        System.out.println
            ("  -h                Print this message and exit.");
        System.out.println
            ("  -likwid           Output likwid energy information as space" +
             " separated data.");
        System.out.println
            ("  -external         Output external power information as space" +
             " separated data.");
    }

    private JSONArray loadMFEvents(String filename)
        throws IOException
    {
        try (FileInputStream fis = new FileInputStream(filename)) {
            return new JSONArray(new JSONTokener(fis));
        } catch (Exception e) {
            System.err.println
                ("Error: Failed to load MF events '" +
                 filename + "'.");
            throw e;
        }
    }

    private void printIfPresent(JSONObject event,
                                String attribute)
    {
        if (event.has(attribute)) {
            System.out.print(" " + event.getDouble(attribute));
        } else {
            System.out.print(" " + 0.0);
        }
    }

    private void processLikwidEvents(JSONArray events)
    {
        System.out.println("# Likwid RAPL energy dissipated in Joules ");
        System.out.println("# timestamp PKG-0 PKG-1 PP0-0 PP0-1 DRAM-0 DRAM-1");
        for (int i = 0; i < events.length(); i++) {
            JSONObject event = events.getJSONObject(i);

            if (!event.has(LIKWID_SOCKET0_PKG)) continue;

            System.out.print("" + event.getDouble(TIMESTAMP));
            printIfPresent(event, LIKWID_SOCKET0_PKG);
            printIfPresent(event, LIKWID_SOCKET1_PKG);
            printIfPresent(event, LIKWID_SOCKET0_PP0);
            printIfPresent(event, LIKWID_SOCKET1_PP0);
            printIfPresent(event, LIKWID_SOCKET0_DRAM);
            printIfPresent(event, LIKWID_SOCKET1_DRAM);
            System.out.println();
        }
    }

    private void processExternalPowerEvents(JSONArray events)
    {
        System.out.println("# External power measurements in Watts ");
        System.out.println("# timestamp SOCKET-0 SOCKET-1 ATX12V GPU-0");
        for (int i = 0; i < events.length(); i++) {
            JSONObject event = events.getJSONObject(i);

            if (!event.has(EXTERNAL_SOCKET0)) continue;

            System.out.print("" + event.getDouble(TIMESTAMP));
            printIfPresent(event, EXTERNAL_SOCKET0);
            printIfPresent(event, EXTERNAL_SOCKET1);
            printIfPresent(event, EXTERNAL_ATX12V);
            printIfPresent(event, EXTERNAL_GPU0);

            System.out.println();
        }
    }

    public static void main(String[] args)
        throws IOException
    {
         new mf_extract().run(args);
    }
}
