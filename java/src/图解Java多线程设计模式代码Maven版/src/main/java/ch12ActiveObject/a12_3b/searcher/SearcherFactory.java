package ch12ActiveObject.a12_3b.searcher;

public class SearcherFactory {
    public static Searcher createSearcher() {
        return new SearcherImpl();
    }
}
