import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.apache.hadoop.io.BooleanWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;


public class MergeAndStep {
    public static class MapClass extends MapReduceBase
    implements Mapper<LongWritable, Text, LongWritable, NodeInfo> {
        public void map(LongWritable key, Text value,
            OutputCollector<LongWritable, NodeInfo> output,
            Reporter reporter) throws IOException
        {
            String line = value.toString();
            StringTokenizer itr = new StringTokenizer(line);
            long node = Long.valueOf(itr.nextToken());
         
            if (isGraph(node)) {
                // Data from graph - list of neighbours
                ArrayList<LongWritable> neighbours = new ArrayList<LongWritable>();
                LongWritable[] neighboursArray;
                NodeInfo onlyNeighboursPair;
                
                while(itr.hasMoreTokens()) {
                    neighbours.add(new LongWritable(new Long(itr.nextToken())));
                }
                neighboursArray = neighbours.toArray(new LongWritable[neighbours.size()]);
                onlyNeighboursPair = new NodeInfo(new LongWritable(Long.MAX_VALUE),
                                                  new BooleanWritable(),
                                                  new LongArrayWritable(neighboursArray));
                
                output.collect(new LongWritable(node), onlyNeighboursPair);
            } else {
                // Data from previous iteration - computed distance
                node = normalizeNode(node);
                Long dist = new Long(itr.nextToken());
                Boolean done = new Boolean(itr.nextToken());
                NodeInfo onlyDistPair = new NodeInfo(new LongWritable(dist),
                                                     new BooleanWritable(done), 
                                                     new LongArrayWritable());
                
                output.collect(new LongWritable(node), onlyDistPair);
            }
        }
        
        private boolean isGraph(long node) {
            return (node >= 0);
        }
        
        private long normalizeNode(long node) {
            if (isGraph(node)) return node;
            else return (node * -1) - 1;
        }
    }

    
    public static class ReduceClass extends MapReduceBase
    implements Reducer<LongWritable, NodeInfo, LongWritable, LongWritable> {
        public void reduce(LongWritable key, Iterator<NodeInfo> values,
                   OutputCollector<LongWritable, LongWritable> output,
                   Reporter reporter) throws IOException
        {
            NodeInfo mergedData = mergeData(values);
            if (mergedData.dist.get() == Long.MAX_VALUE)
                return;

            output.collect(key, new LongWritable((mergedData.dist.get() * -1) - 1)); // Propagate original dist.
            
            if (mergedData.done.get()) // Step already done, ignore.
                return;
            
            LongWritable[] neighbours = (LongWritable[])mergedData.neighbours.toArray();
            for (LongWritable n : neighbours) {
                LongWritable new_dist = new LongWritable(mergedData.dist.get() + 1);
                output.collect(n, new_dist);
            } 
        }

        /* Iterates over values and merges partial data into a single NodeInfo object */
        private static NodeInfo mergeData(Iterator<NodeInfo> values) {
            NodeInfo result = new NodeInfo();
            while (values.hasNext()) {
                NodeInfo pair = values.next();
                if (pair.dist.get() != Long.MAX_VALUE) {
                    result.dist.set(pair.dist.get());
                    result.done.set(pair.done.get());
                } else {
                    result.neighbours.set(pair.neighbours.get());
                }
            }
            return result;
        }
    }
}