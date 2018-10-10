package appendixG;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.List;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.videoio.VideoCapture;
import org.tensorflow.DataType;
import org.tensorflow.Graph;
import org.tensorflow.Output;
import org.tensorflow.Session;
import org.tensorflow.Tensor;
import org.tensorflow.types.UInt8;


public class OpencvWebCamWithTensorflow
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[]) throws InterruptedException, IOException {
        VideoCapture capture = new VideoCapture();
        capture.open(0);
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("從webcam讀取影像至Java Swing視窗");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        if (!capture.isOpened())
        {
            System.out.println("Error");
        } else
        {
            Mat webcam_image = new Mat();
            capture.read(webcam_image);
            frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
            String resrult;

            while (true) {
                Thread.sleep(1000);

                capture.read(webcam_image);
                resrult = mainP(BufferedImage2ByteArray(matToBufferedImage(webcam_image)));
                //System.out.println("從webcam讀取影像至Java Swing視窗");
                System.out.println(resrult.toString());
                panel1.setimagewithMat(webcam_image);
                frame1.repaint();
            }
        }
    }
    public  static String mainP(byte[]  imageBytes)
    {
        String result = "";

        String modelDir = "c:\\tf";

        byte[] graphDef = readAllBytesOrExit(Paths.get(modelDir,
                                             "tensorflow_inception_graph.pb"));
        List<String> labels =
            readAllLinesOrExit(Paths.get(modelDir,
                                         "imagenet_comp_graph_label_strings.txt"));

        try (Tensor<Float> image = constructAndExecuteGraphToNormalizeImage(
                                           imageBytes)) {
            float[] labelProbabilities = executeInceptionGraph(graphDef, image);
            int bestLabelIdx = maxIndex(labelProbabilities);
            result = "預測影像為: " + labels.get(bestLabelIdx) + " (機率為" +
                     (labelProbabilities[bestLabelIdx] * 100f) + "%)";
        }

        return result;
    }

    private static Tensor<Float> constructAndExecuteGraphToNormalizeImage(
        byte[] imageBytes)
    {
        try (Graph g = new Graph()) {
            GraphBuilder b = new GraphBuilder(g);
            final int H = 224;
            final int W = 224;
            final float mean = 117f;
            final float scale = 1f;

            final Output<String> input = b.constant("input", imageBytes);
            final Output<Float> output =
                b.div(
                    b.sub(
                        b.resizeBilinear(
                            b.expandDims(
                                b.cast(b.decodeJpeg(input, 3), Float.class),
                                b.constant("make_batch", 0)),
                            b.constant("size", new int[] {H, W})),
                        b.constant("mean", mean)),
                    b.constant("scale", scale));

            try (Session s = new Session(g)) {
                return s.runner().fetch(output.op().name()).run().get(0).expect(Float.class);
            }
        }
    }

    private static float[] executeInceptionGraph(byte[] graphDef,
            Tensor<Float> image)
    {
        try (Graph g = new Graph()) {
            g.importGraphDef(graphDef);

            try (Session s = new Session(g);
                     Tensor<Float> result =
                         s.runner().feed("input",
                                         image).fetch("output").run().get(0).expect(Float.class)) {
                final long[] rshape = result.shape();

                if (result.numDimensions() != 2 || rshape[0] != 1) {
                    throw new RuntimeException(
                        String.format(
                            "Expected model to produce a [1 N] shaped tensor where N is the number of labels, instead it produced one with shape %s",
                            Arrays.toString(rshape)));
                }

                int nlabels = (int) rshape[1];
                return result.copyTo(new float[1][nlabels])[0];
            }
        }
    }

    private static int maxIndex(float[] probabilities)
    {
        int best = 0;

        for (int i = 1; i < probabilities.length; ++i) {
            if (probabilities[i] > probabilities[best]) {
                best = i;
            }
        }

        return best;
    }

    private static byte[] readAllBytesOrExit(Path path)
    {
        try {
            return Files.readAllBytes(path);
        } catch (IOException e) {
            System.err.println("Failed to read [" + path + "]: " + e.getMessage());
            System.exit(1);
        }

        return null;
    }

    private static List<String> readAllLinesOrExit(Path path)
    {
        try {
            return Files.readAllLines(path, Charset.forName("UTF-8"));
        } catch (IOException e) {
            System.err.println("Failed to read [" + path + "]: " + e.getMessage());
            System.exit(0);
        }

        return null;
    }

    // In the fullness of time, equivalents of the methods of this class should be auto-generated from
    // the OpDefs linked into libtensorflow_jni.so. That would match what is done in other languages
    // like Python, C++ and Go.
    static class GraphBuilder
    {
        GraphBuilder(Graph g)
        {
            this.g = g;
        }

        Output<Float> div(Output<Float> x, Output<Float> y)
        {
            return binaryOp("Div", x, y);
        }

        <T> Output<T> sub(Output<T> x, Output<T> y)
        {
            return binaryOp("Sub", x, y);
        }

        <T> Output<Float> resizeBilinear(Output<T> images, Output<Integer> size)
        {
            return binaryOp3("ResizeBilinear", images, size);
        }

        <T> Output<T> expandDims(Output<T> input, Output<Integer> dim)
        {
            return binaryOp3("ExpandDims", input, dim);
        }

        <T, U> Output<U> cast(Output<T> value, Class<U> type)
        {
            DataType dtype = DataType.fromClass(type);
            return g.opBuilder("Cast", "Cast")
                   .addInput(value)
                   .setAttr("DstT", dtype)
                   .build()
                   .<U>output(0);
        }

        Output<UInt8> decodeJpeg(Output<String> contents, long channels)
        {
            return g.opBuilder("DecodeJpeg", "DecodeJpeg")
                   .addInput(contents)
                   .setAttr("channels", channels)
                   .build()
                   .<UInt8>output(0);
        }

        <T> Output<T> constant(String name, Object value, Class<T> type)
        {
            try (Tensor<T> t = Tensor.<T>create(value, type)) {
                return g.opBuilder("Const", name)
                       .setAttr("dtype", DataType.fromClass(type))
                       .setAttr("value", t)
                       .build()
                       .<T>output(0);
            }
        }
        Output<String> constant(String name, byte[] value)
        {
            return this.constant(name, value, String.class);
        }

        Output<Integer> constant(String name, int value)
        {
            return this.constant(name, value, Integer.class);
        }

        Output<Integer> constant(String name, int[] value)
        {
            return this.constant(name, value, Integer.class);
        }

        Output<Float> constant(String name, float value)
        {
            return this.constant(name, value, Float.class);
        }

        private <T> Output<T> binaryOp(String type, Output<T> in1, Output<T> in2)
        {
            return g.opBuilder(type, type).addInput(in1).addInput(in2).build().<T>output(0);
        }

        private <T, U, V> Output<T> binaryOp3(String type, Output<U> in1,
                                              Output<V> in2)
        {
            return g.opBuilder(type, type).addInput(in1).addInput(in2).build().<T>output(0);
        }
        private Graph g;
    }
    public static BufferedImage matToBufferedImage(Mat matrix)
    {
        int cols = matrix.cols();
        int rows = matrix.rows();
        int elemSize = (int) matrix.elemSize();
        byte[] data = new byte[cols * rows * elemSize];
        int type;
        matrix.get(0, 0, data);

        switch (matrix.channels()) {
        case 1:
            type = BufferedImage.TYPE_BYTE_GRAY;
            break;

        case 3:
            type = BufferedImage.TYPE_3BYTE_BGR;
            // bgr to rgb
            byte b;

            for (int i = 0; i < data.length; i = i + 3) {
                b = data[i];
                data[i] = data[i + 2];
                data[i + 2] = b;
            }

            break;

        default:
            return null;
        }

        BufferedImage image2 = new BufferedImage(cols, rows, type);
        image2.getRaster().setDataElements(0, 0, cols, rows, data);
        return image2;
    }
    public static byte[] BufferedImage2ByteArray(BufferedImage bi) throws
    IOException {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        ImageIO.write(bi, "jpg", baos);
        baos.flush();
        byte[] imageInByte = baos.toByteArray();
        baos.close();
        return imageInByte;

    }
}
