tradeLog: ([] pos_id:1 2 3 4 5 6; sym:`SWAP1`SWAP2`SWAP1`CDS_ACME`SWAP2`SWAP1; entry_date:2026.08.05 2026.08.05 2026.08.06 2026.08.06 2026.08.07 2026.08.07; entry_time:09:15:00.000 09:40:00.000 11:02:00.000 13:10:00.000 15:05:00.000 10:20:00.000; notional:1000000 5000000 2000000 3000000 4000000 1500000)

curves: ([] sym:`SWAP1`SWAP1`SWAP2`SWAP2`SWAP1`CDS_ACME`SWAP1`SWAP2`SWAP1; date:2026.08.05 2026.08.06 2026.08.05 2026.08.07 2026.08.06 2026.08.06 2026.08.07 2026.08.07 2026.08.07; time:09:00:00.000 13:45:00.000 09:00:00.000 13:30:00.000 16:00:00.000 09:00:00.000 09:00:00.000 11:00:00.000 14:00:00.000; rate:1.250 1.265 1.100 1.115 1.270 0.0325 1.280 1.130 1.290)
tradeLog: ("ISDTF"; enlist ",") 0: `:tradeLog.csv
curves: ("SDTF"; enlist ",") 0: `:curves.csv
tradeLog: update entry_ts: entry_date + entry_time from tradeLog

curves: update ts: date + time from curves

bookSnapshot:{[snapTs] bookAtSnap: select from tradeLog where entry_ts<=snapTs; curveAsOf: select ts:last ts, rate:last rate by sym from curves where ts<=snapTs; snap: aj[`sym; `sym xasc bookAtSnap; `sym xasc 0!curveAsOf]; update mtm_value: notional * rate % 100 from snap}

bookSnapshot[2026.08.06D14:00:00.000000000]
