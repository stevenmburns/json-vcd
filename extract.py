import vcdvcd
import argparse
import json

sigs = {
  'clock'  : 'test.GCDInner.clock',
  'reset'   : 'test.GCDInner.reset',
  'io_e' : 'test.GCDInner.io_e',
  'io_a'  : 'test.GCDInner.io_a[15:0]',
  'io_b' : 'test.GCDInner.io_b[15:0]',
  'io_v'  : 'test.GCDInner.io_v',
  'io_z' : 'test.GCDInner.io_z[15:0]'
}

def main( ifn, ofn):
  g = vcdvcd.VCDVCD( ifn, signals=list(sigs.values()))

  times = {}

  for k, v in sigs.items():
    for t, value in g[v].tv:
      if t not in times: 
        times[t] = []
      times[t].append( (k, value))
    
  result = []
  for t in sorted(times.keys()):
    changes = []
    for variable, value in times[t]:
      changes.append( { "variable": variable, "value": value})
    result.append( { "time": t, "changes": changes})  
    

  with open( ofn, "wt") as ofp:
    json.dump( result, ofp, indent=2)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Convert vcd to JSON.")


    parser.add_argument( "-if", "--vcd_filename", type=str, default="GCDInner.vcd", help="Filename of .vcd file.")

    parser.add_argument( "-of", "--json_filename", type=str, default="out.json", help="Filename of .json file.")

    args = parser.parse_args()

    main( args.vcd_filename, args.json_filename)
